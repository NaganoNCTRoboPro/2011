#ifndef __SCI_H__
#define __SCI_H__

// UART 設定用マクロ
#define	UART_TXCIE		0x40	// トランスミットエンドインタラプトイネーブル
#define UART_UDRIE		0x20	// トランスミットインタラプトイネーブル
#define UART_RXCIE		0x80	// レシーブインタラプトイネーブル

#define	UART_RE			0x10	// 受信許可
#define UART_TE			0x08	// 送信許可

extern int uart_init(unsigned char scr_value, unsigned long speed);
extern void uart_setbuffer(unsigned char *buf, int size);
extern int uart_putchar(unsigned char c);
extern int uart_getchar(unsigned char *c);
extern int uart_buf_putchar(unsigned char c);
//extern void int_uart_tx(void) __attribute__ ((interrupt_handler));


#define PERIPHERAL_CLOCK_FREQ		8000000// [Hz]

// utility definition
#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)


#endif
