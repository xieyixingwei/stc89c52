#include <reg52.h>
#include <string.h>
#include "c51.h"

#define BIT(x)  (1 << (x))

#ifndef MIN
#define MIN(a, b)    (((a) < (b)) ? (a) : (b))
#endif

struct loop_buf {
    u16 wp;
    u16 rp;
    u16 size;
    u8* buf;
};

static struct loop_buf g_rxbuf = {0, 0 , 0, NULL}; 

/**
 * @baud   baud rate
 * 
 */
void c51_uart_init(u16 baud, u8* rxbuf, u16 size)
{
    g_rxbuf.buf = rxbuf;
    g_rxbuf.size = size;

    /* diable uart receive */
    SCON &= ~BIT(4);

    /* set SCON[7:6] = b01 */
    SCON &= ~(BIT(7) | BIT(6));
    SCON |= BIT(6);

    /* set timer1 to 8bit auto-load mode */
    TMOD = 0x20;

    TH1 = 253;
    TL1 = 253;

    ET1 = 0;        /* close timer1 overflow interrupt */
	TR1 = 1;        /* start timer1 count */
 
    ES = 1; /* enable uart interrupt */
    EA = 1; /* enable cpu interrupt */

    /* enable uart receive */
    SCON |= BIT(4);
}

u16 c51_uart_send(const u8* buf, u16 len)
{
    u16 i = 0;
    TI = 0;

    for(i = 0; i < len; i++)
    {
        SBUF = buf[i];
        while(0 == TI);
        TI = 0;
    }

    return i;
}

u16 c51_uart_recv(u8* buf, u16 size)
{
    u16 len = 0;
    u16 remainLen = 0;
    u16 wp = g_rxbuf.wp;

    if(wp == g_rxbuf.rp)
    {
        return 0;
    }

    if(wp > g_rxbuf.rp)
    {
        len = MIN(wp - g_rxbuf.rp, size);
        memcpy((char*)buf, (char*)&g_rxbuf.buf[g_rxbuf.rp], len);
        g_rxbuf.rp += len;
        return len;
    }

    if(wp < g_rxbuf.rp)
    {
        len = MIN(g_rxbuf.size - g_rxbuf.rp, size);
        memcpy((char*)buf, (char*)&g_rxbuf.buf[g_rxbuf.rp], len);
        g_rxbuf.rp += len;

        remainLen = MIN(wp, size - len);
        memcpy((char*)&buf[len], (char*)g_rxbuf.buf, remainLen);
        g_rxbuf.rp += remainLen;
        return len + remainLen;
    }

    return 0;
}

void uart_irq(void) interrupt 4
{
    EA = 0;
P1 ^= 0x01;
    if(RI)
    {
        RI = 0;
        if(NULL != g_rxbuf.buf)
        {
            g_rxbuf.buf[g_rxbuf.wp++] = SBUF;
            if(g_rxbuf.wp == g_rxbuf.size)
            {
                g_rxbuf.wp = 0;
            }
        }
    }

    EA = 1;
}