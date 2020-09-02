#include <reg52.h>

#include "../lib/c51.h"


void delay(volatile u16 n)
{
    while(n--);
}

void main(void)
{
    u8 buf[32] = {0};
    u8 rxbuf[32] = {0};
    u16 len = 0;

    c51_uart_init(9600, rxbuf, 32);
    
    while(1)
    {
        len = c51_uart_recv(buf, 32);
        if(len)
            c51_uart_send(buf, len);
        
    }
}