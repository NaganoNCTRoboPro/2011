#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"
#include "ringbuffer.h"
#include "uart.h"

static RingBuffer __uartbuf[UART_NUM];
static char _uart_interrupt_status;

#define start_uart0tx_interrupt()	sbi(UCSR0B,UDRIE0)
#define stop_uart0tx_interrupt()	cbi(UCSR0B,UDRIE0)
 
#define start_uart1tx_interrupt()	sbi(UCSR1B,UDRIE1)
#define stop_uart1tx_interrupt()	cbi(UCSR1B,UDRIE1)

/// <summary>
/// UART 初期化関数
/// </summary>
/// <param name="uart_no">UARTの選択 0 または 1</param>
/// <param name="option">uart.h にあるUART設定用マクロを使う</param>
/// <param name="speed">通信速度.ボーレート[bps]を入れる</param>
int uart_init(int uart_no, unsigned char option, unsigned long speed)
{
	volatile uint32_t i;
	uint32_t b;
	uint32_t x, y;
	
	x = PERIPHERAL_CLOCK_FREQ * 1000000;
	y = 8 * speed;	// 倍速動作設定のため、ボーレートに 8 をかける

	b = x / y -1;

		
	switch( uart_no ){
		case 0:
			sbi(UCSR0A,U2X0);			// 倍速許可

			UCSR0B = 0x00;
			UCSR0C |= 0x06;

			

			for(i=0; i<5000; i++);		// Wait
			
			UCSR0B = option;					

			UBRR0H = (unsigned char)(b>>8);
	 		UBRR0L = (unsigned char)b;		

			UCSR0A;						// DummyRead
			UCSR0A &= 0xe3;				// Clear Error Flag
			break;
		case 1:
			sbi(UCSR1A,U2X1);			// 倍速許可

			UBRR1H = (unsigned char)(b>>8);
	 		UBRR1L = (unsigned char)b;	 

			for(i=0; i<5000; i++);		// Wait
			
			UCSR1B |= option;
			UCSR1A;						// DummyRead
			UCSR1A &= 0xe3;				// Clear Error Flag
			break;
		default:
			return -1;
	}

	return 0;
}

void uart_setbuffer(int uart_no, unsigned char *buf, int size)
{
	if( uart_no<0 || uart_no>=UART_NUM )	return;
	
	RingInit(&(__uartbuf[uart_no]), buf, size);
}

unsigned char uart0_getchar(void)
{
	while( !bit_is_set(UCSR0A,RXC0) );
	cbi(UCSR0A,RXC0);
	return UDR0;
}

int uart0_putchar(char c)
{
/*#if (UART0_LINE_FEED == CR_CODE | LF_CODE)		// CRLF
	if( c == '\n' ){
		while( !bit_is_set(UCSR0A,UDRE0) );
		UDR0 = '\r';
		cbi(UCSR0A,UDRE0);
	}
#elif (UART0_LINE_FEED == CR_CODE)				// CR
	if( c == '\n' )		c = '\r';
#elif (UART0_LINE_FEED == LF_CODE)
#endif*/
	while( !bit_is_set(UCSR0A,UDRE0) );
	UDR0 = c;
	cbi(UCSR0A,UDRE0);
	
	return 0;
}

int uart1_putchar(char c)
{
#if (UART1_LINE_FEED == CR_CODE | LF_CODE)		// CRLF
	if( c == '\n' ){
		while( !bit_is_set(UCSR1A,UDRE1) );
		UDR1 = '\r';
		cbi(UCSR1A,UDRE1);
	}
#elif (UART1_LINE_FEED == CR_CODE)				// CR
	if( c == '\n' )		c = '\r';
#elif (UART1_LINE_FEED == LF_CODE)
#endif
	while( !bit_is_set(UCSR1A,UDRE1) );
	UDR1 = c;
	cbi(UCSR1A,UDRE1);

	return 0;
}

int uart0_buf_putchar(char c)
{
	int ret;
	
	ret = 0;
	_uart_interrupt_status = SREG;
	cli();
	
#if (UART0_LINE_FEED == CR_CODE | LF_CODE)		// CRLF
	if( c == '\n' ){
		if( !RingPut(&__uartbuf[0], '\r') ){
			ret = -1;	// Buffer Full
			goto _exit_uart0_buf_putchar;	
		}
	}
#elif (UART0_LINE_FEED == CR_CODE)				// CR
	if( c == '\n' )		c = '\r';
#elif (UART0_LINE_FEED == LF_CODE)
#endif

	if( !RingPut(&__uartbuf[0], c) ){
		ret = -1;	// Buffer Full
	}
	start_uart0tx_interrupt();
	
#if (UART0_LINE_FEED == CR_CODE | LF_CODE)		// CRLF
_exit_uart0_buf_putchar:
#endif
	SREG = _uart_interrupt_status;

	return ret;
}

int uart1_buf_putchar(char c)
{
	int ret;
	
	ret = 0;
	_uart_interrupt_status = SREG;
	cli();
	
#if (UART1_LINE_FEED == CR_CODE | LF_CODE)		// CRLF
	if( c == '\n' ){
		if( !RingPut(&__uartbuf[1], '\r') ){
			ret = -1;	// Buffer Full
			goto _exit_uart1_buf_putchar;	
		}
	}
#elif (UART1_LINE_FEED == CR_CODE)				// CR
	if( c == '\n' )		c = '\r';
#elif (UART1_LINE_FEED == LF_CODE)
#endif
	if( !RingPut(&__uartbuf[1], c) ){
		ret = -1;	// Buffer Full
	}
	start_uart1tx_interrupt();

	
#if (UART1_LINE_FEED == CR_CODE | LF_CODE)		// CRLF
_exit_uart1_buf_putchar:
#endif
	SREG = _uart_interrupt_status;

	return ret;
}


ISR(USART0_UDRE_vect)
{
	unsigned char c;

	if( RingGet(&__uartbuf[0], &c) ){
		UDR0 = c;
		cbi(UCSR0A,UDRE0);			// UDREクリア
	}else{
		stop_uart0tx_interrupt();
		cbi(UCSR0A,UDRE0);
	}
}

ISR(USART1_UDRE_vect)
{
	unsigned char c;
	
	if( RingGet(&__uartbuf[1], &c) ){
		UDR1 = c;
		cbi(UCSR1A,UDRE1);			// UDREクリア
	}else{
		stop_uart1tx_interrupt();
		cbi(UCSR1A,UDRE1);
	}
}

