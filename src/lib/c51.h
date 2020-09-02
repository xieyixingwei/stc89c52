#ifndef __C51_H__
#define __C51_H__

#define FOSC  12000000  /* system frequency */

typedef unsigned int  u16;
typedef unsigned char u8;
typedef int           int16;
typedef char          int8;

#ifndef NULL
#define NULL 0
#endif

void c51_uart_init(u16 baud, u8* rxbuf, u16 size);
u16 c51_uart_send(const u8* buf, u16 len);
u16 c51_uart_recv(u8* buf, u16 size);

#endif /* __C51_H__ */
