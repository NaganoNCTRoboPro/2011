#ifndef __SCI_H__
#define __SCI_H__

#include <common.h>
#include <ringbuffer.h>

// UART 設定用マクロ
#define	UART_TXCIE		0x40	// トランスミットエンドインタラプトイネーブル
#define UART_UDRE		0x20	// トランスミットインタラプトイネーブル
#define UART_RXCIE		0x80	// レシーブインタラプトイネーブル

#define	UART_RE			0x10	// 受信許可
#define UART_TE			0x08	// 送信許可
#define TX 1
#define RX 0

extern int uart_init(int uart_no, unsigned char scr_value, unsigned long speed);
extern void uart_setbuffer(int uart_no, int dir, unsigned char *buf, int size);
extern int uart0_putchar(unsigned char c);
unsigned char uart0_getchar(void);
extern int uart1_putchar(char c);
extern int uart0_buf_putchar(char c);
extern int uart1_buf_putchar(char c);
//extern void int_uart0_tx(void) __attribute__ ((interrupt_handler));
//extern void int_uart1_tx(void) __attribute__ ((interrupt_handler));

extern RingBuffer __uartbuf[UART_NUM][2];

#define LF_CODE		(1)
#define CR_CODE		(2)

#define UART0_LINE_FEED	(0)
#define UART1_LINE_FEED	(CR_CODE)


#endif
