#ifndef __SCI_H__
#define __SCI_H__

#include <common.h>
#include <ringbuffer.h>

// UART �ݒ�p�}�N��
#define	UART_TXCIE		0x40	// �g�����X�~�b�g�G���h�C���^���v�g�C�l�[�u��
#define UART_UDRE		0x20	// �g�����X�~�b�g�C���^���v�g�C�l�[�u��
#define UART_RXCIE		0x80	// ���V�[�u�C���^���v�g�C�l�[�u��

#define	UART_RE			0x10	// ��M����
#define UART_TE			0x08	// ���M����

//baud rate(20MHz)
#define BR_1200	1041
#define BR_2400  520
#define BR_4800  259
#define BR_9600	 129
#define BR_14400  86
#define BR_19200  64
#define BR_28800  42
#define BR_38400  32
#define BR_57600  21
#define BR_76800  15
#define BR_115200 10
#define BR_230400  4
#define BR_250000  4
#define BR_500000  1
#define BR_1000000 0	

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

extern RingBuffer __uartbuf[UART_NUM];

#endif
