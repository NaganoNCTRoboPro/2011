#include <avr/io.h>
#include <avr/interrupt.h>
#include "ringbuffer.h"
#include "uart.h"

static RingBuffer __uartbuf;
static char _uart_interrupt_status;

#define start_uart_tx_interrupt()	sbi(UCSR0B,UDRIE0)
#define stop_uart_tx_interrupt()	cbi(UCSR0B,UDRIE0)

/// <summary>
/// UART 初期化関数
/// </summary>
/// <param name="uart_no">UARTの選択 0 または 1</param>
/// <param name="option">uart.h にあるUART設定用マクロを使う</param>
/// <param name="speed">通信速度.ボーレート[bps]を入れる</param>
int uart_init(unsigned char option, unsigned long speed)
{
	volatile unsigned int i;
	unsigned int b;

	b = PERIPHERAL_CLOCK_FREQ / 8 / speed -1;

    sbi(UCSR0A,U2X0);			// 倍速許可

    UCSR0B = 0x00;
    UCSR0C |= 0x06;

    for(i=0; i<5000; i++);		// Wait

    UCSR0B = option;

    UBRR0H = (unsigned char)(b>>8);
    UBRR0L = (unsigned char)b;

    UCSR0A;						// DummyRead
    UCSR0A &= 0xe3;				// Clear Error Flag

	return 0;
}

void uart_setbuffer(unsigned char *buf, int size)
{
	RingInit(&__uartbuf, buf, size);
}

int uart_getchar(unsigned char *c)
{
	while( !bit_is_set(UCSR0A,RXC0) );
	cbi(UCSR0A,RXC0);
	*c = UDR0;
	return 0;
}

int uart_putchar(unsigned char c)
{
	while( !bit_is_set(UCSR0A,UDRE0) );
	UDR0 = c;
	cbi(UCSR0A,UDRE0);

	return 0;
}

int uart_buf_putchar(unsigned char c)
{
	int ret;

	ret = 0;
	_uart_interrupt_status = SREG;
	cli();

	if( !RingPut(&__uartbuf, c) ){
		ret = -1;	// Buffer Full
	}
	start_uart_tx_interrupt();

	SREG = _uart_interrupt_status;

	return ret;
}

ISR(USART_UDRE_vect)
{
	unsigned char c;

	if( RingGet(&__uartbuf, &c) ){
		UDR0 = c;
		cbi(UCSR0A,UDRE0);			// UDREクリア
	}else{
		stop_uart_tx_interrupt();
		cbi(UCSR0A,UDRE0);
	}
}

