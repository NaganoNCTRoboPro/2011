#ifndef __SCI_H__
#define __SCI_H__

#include <ringbuffer.h>

// UART 設定用マクロ
#define	UART_TXCIE		0x40	// トランスミットエンドインタラプトイネーブル
#define UART_UDRE		0x20	// トランスミットインタラプトイネーブル
#define UART_RXCIE		0x80	// レシーブインタラプトイネーブル

#define	UART_RE			0x10	// 受信許可
#define UART_TE			0x08	// 送信許可

//baud rate(20MHz)
#define BR_1200		416
#define BR_2400		207
#define BR_4800  	103
#define BR_9600	 	51
#define BR_14400  	34
#define BR_19200  	25
#define BR_28800  	16
#define BR_38400  	12
#define BR_57600  	8
#define BR_76800  	6
#define BR_115200 	3
#define BR_230400  	1
#define BR_250000  	1
#define BR_500000  	0
#define BR_1000000  0	

extern int uart_init(int uart_no, unsigned char option, unsigned int baud);

extern void uart_setbuffer(int uart_no, unsigned char *buf, int size);
extern int uart0_putchar(char c);
unsigned char uart0_getchar(void);
extern int uart1_putchar(char c);
extern int uart0_buf_putchar(char c);
extern int uart1_buf_putchar(char c);
//extern void int_uart0_tx(void) __attribute__ ((interrupt_handler));
//extern void int_uart1_tx(void) __attribute__ ((interrupt_handler));

#define LF_CODE		(1)
#define CR_CODE		(2)

#define UART0_LINE_FEED	(0)
#define UART1_LINE_FEED	(CR_CODE)


#endif
