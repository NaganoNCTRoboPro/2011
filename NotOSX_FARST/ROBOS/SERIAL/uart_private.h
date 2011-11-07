#ifndef UART_PRIVATE_H
#define UART_PRIVATE_H

#define UART0_TRANSMITTING	( ! ( UCSR0A & _BV(UDRE0) ) ) 
#define UART1_TRANSMITTING	( ! ( UCSR1A & _BV(UDRE1) ) ) 
#define UART0_RECEIVING		( ! ( UCSR0A & _BV(RXC0) ) ) 
#define UART1_RECEIVING		( ! ( UCSR1A & _BV(RXC1) ) )


enum {
	OneFrameIs5Bit = 0x00,
	OneFrameIs6Bit = 0x02,
	OneFrameIs7Bit = 0x04,
	OneFrameIs8Bit = 0x06
};

#endif
