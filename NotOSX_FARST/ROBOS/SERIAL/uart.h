#ifndef UART_H
#define UART_H

#define FREQ 8 //(MHz)

enum UARTNumber {
	UART0,
	UART1
};
enum UARTMode {
	StopBitIs1Bit = 0x00,
	StopBitIs2Bit = 0x08,
	NonParity = 0x00,
	EvenParity = 0x20,
	OddParity = 0x30
};

enum UARTAction {
	ReceiveCompleteInteruptEnable = 0x80,
	TransmitCompleteInteruptEnable = 0x40,
	DataRegisterEmptyInteruptEnable = 0x20,
	ReceiveEnable = 0x10,
	TransmitEnable = 0x08
};

void initUART(enum UARTNumber no, uint8_t mode, uint8_t act,uint32_t speed );
void uart0Put(uint8_t value);
void uart1Put(uint8_t value);
uint8_t uart0Get(void);
uint8_t uart1Get(void);
void uart0Puts(uint8_t* values, uint8_t size);
void uart1Puts(uint8_t* values, uint8_t size);
void setTransmitCompleteInterruptFunc(enum UARTNumber no, volatile void (*f)(void));
void setDataRegisterEmptyFunc(enum UARTNumber no, volatile void (*f)(void));
 void setReceiveCompleteInterruptFunc(enum UARTNumber no, volatile void (*f)(void));

#endif //UART_H
