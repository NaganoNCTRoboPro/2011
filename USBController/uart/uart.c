#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "ringbuffer.h"
#include "uart.h"
#include <wait.h>

#define UART_NUM		2

// utility definition
#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)

RingBuffer __uartbuf[UART_NUM];
static char _uart_interrupt_status;

#define start_uart0tx_interrupt()	sbi(UCSR0B,UDRIE0)
#define stop_uart0tx_interrupt()	cbi(UCSR0B,UDRIE0)
#define start_uart1tx_interrupt()	sbi(UCSR1B,UDRIE1)
#define stop_uart1tx_interrupt()	cbi(UCSR1B,UDRIE1)

int uart_init(int uart_no,unsigned char option,unsigned int baud)
{
	int i;
	switch( uart_no ){
		case 0:
			UCSR0B = 0x00;
			UCSR0C |= 0x06;
			for(i=0; i<5000; i++);		// Wait
			UCSR0B = option;					
			UBRR0H = (unsigned char)(baud>>8);
	 		UBRR0L = (unsigned char)baud;		
			UCSR0A;						// DummyRead
			UCSR0A &= 0xe3;				// Clear Error Flag
			UDR0 = 0;
			break;
		case 1:
			UBRR1H = (unsigned char)(baud>>8);
	 		UBRR1L = (unsigned char)baud;	 
			for(i=0; i<5000; i++);		// Wait
			UCSR1B |= option;
			UCSR1A;						// DummyRead
			UCSR1A &= 0xe3;				// Clear Error Flag
			UDR1 = 0;
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
/*
#if (UART0_LINE_FEED == CR_CODE | LF_CODE)		// CRLF
	if( c == '\n' ){
		while( !bit_is_set(UCSR0A,UDRE0) );
		UDR0 = '\r';
		cbi(UCSR0A,UDRE0);
	}
#elif (UART0_LINE_FEED == CR_CODE)				// CR
	if( c == '\n' )		c = '\r';
#elif (UART0_LINE_FEED == LF_CODE)
#else
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


