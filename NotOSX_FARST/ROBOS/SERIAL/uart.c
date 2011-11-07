 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "uart.h"
 #include "uart_private.h"
 
/**
 * UART���W�X�^�̏����� �i8bit �p���e�B�Ȃ��j
 * @param {no:UARTNumber} UART�̔ԍ�
 * @param {mode:uint8_t} UART�̃��[�h�ݒ�l�i�X�g�b�v�r�b�g�C�p���e�B�r�b�g�j
 * @param {act:uint8_t} UART�̓���̐ݒ�l�i���M�C��M�C���荞�݂Ȃǁj
 * @param {speed:uint32} UART�̒ʐM���x
 */
void initUART(enum UARTNumber no, uint8_t mode, uint8_t act,uint32_t speed )
{
	uint32_t baud = ( ( ( FREQ * 1000000 ) >> 4 ) / speed ) - 1;
	
	switch ( no )
		{
			case UART0:
				UCSR0C = mode | OneFrameIs8Bit;
				UCSR0B = act;
				UBRR0H = (uint8_t)(baud>>8);
		 		UBRR0L = (uint8_t)baud;
				break;
			case UART1:
				UCSR1C = mode | OneFrameIs8Bit;
				UCSR1B = act;
				UBRR1H = (uint8_t)(baud>>8);
		 		UBRR1L = (uint8_t)baud;
				break;
			default:
				break;
		}
}
 
/**
 * 1Byte���M����
 * @param {value:uint8_t} ���M����f�[�^
 */
void uart0Put(uint8_t value)
{
	while ( UART0_TRANSMITTING );
	UDR0 = value;
}
void uart1Put(uint8_t value)
{
	while ( UART1_TRANSMITTING );
	UDR1 = value;
} 
 
/**
 * 1Byte��M����
 * @return {uint8_t} ��M�����f�[�^
 */
uint8_t uart0Get(void)
{
	while ( UART0_RECEIVING );
	return UDR0;
}
uint8_t uart1Get(void)
{
	while ( UART1_RECEIVING );
	return UDR1;
}

/**
 * ����Byte���M����
 * @param {values:uint8_t*} ����f�[�^�Q
 * @param {size:uint8_t} ����f�[�^�T�C�Y
 */
void uart0Puts(uint8_t* values, uint8_t size)
{
	for ( ; size != 0; size-- )
		{
			uart0Put(*values++);
		}
}

void uart1Puts(uint8_t* values, uint8_t size)
{
	for ( ; size != 0; size-- )
		{
			uart1Put(*values++);
		}
}
/**
 * ����Byte��M����
 */


/**
 * ���M�������荞�ݓ���Ϗ��p�֐��|�C���^
 * @type {void(*)(void)}
 */
 static volatile void (*transComplete0)(void);
 static volatile void (*transComplete1)(void);

/**
 * ���M�������荞�ݓ���Ϗ��p�֐��|�C���^�Z�b�^�[�֐�
 * @param {no:UARTNumber} UART�̔ԍ�
 * @param {f:void(*)(void)} �֐��|�C���^
 */
void setTransmitCompleteInterruptFunc(enum UARTNumber no, volatile void (*f)(void))
{
	if ( no == UART0 )
		transComplete0 = f;
	else if ( no == UART1 )
		transComplete1 = f;
}

/**
 * ���M�f�[�^���W�X�^�󂫊��荞�ݓ���Ϗ��p�֐��|�C���^
 * @type {void(*)(void)}
 */
static volatile void (*dataRegisterEmpty0)(void);
static volatile void (*dataRegisterEmpty1)(void);
/**
 * ���M�f�[�^���W�X�^�󂫊��荞�ݓ���Ϗ��p�֐��|�C���^�Z�b�^�[�֐�
 * @param {no:UARTNumber} UART�̔ԍ�
 * @param {f:void(*)(void)} �֐��|�C���^
 */
void setDataRegisterEmptyFunc(enum UARTNumber no, volatile void (*f)(void))
{
	if ( no == UART0 )
		dataRegisterEmpty0 = f;
	else if ( no == UART1 )
		dataRegisterEmpty1 = f;
}

/**
 * ��M�������荞�ݓ���Ϗ��p�֐��|�C���^
 * @type {void(*)(void)}
 */
 static volatile void (*receiveComplete0)(void);
 static volatile void (*receiveComplete1)(void);

/**
 * ��M�������荞�ݓ���Ϗ��p�֐��|�C���^�Z�b�^�[�֐�
 * @param {no:UARTNumber} UART�̔ԍ�
 * @param {f:void(*)(void)} �֐��|�C���^
 */
 void setReceiveCompleteInterruptFunc(enum UARTNumber no, volatile void (*f)(void))
 {
 	if ( no == UART0 )
		receiveComplete0 = f;
	else if ( no == UART1 )
		receiveComplete1 = f;
 }

/**
 * ���M�������荞��
 */
ISR (USART0_TX_vect)
{
	transComplete0();
}

ISR (USART1_TX_vect)
{
	transComplete1();
}
 
/**
 * ���M�f�[�^���W�X�^�󂫊��荞��
 */
ISR (USART0_UDRE_vect)
{
	dataRegisterEmpty0();
}

ISR (USART1_UDRE_vect)
{
	dataRegisterEmpty1();
}

/**
 * ��M�������荞��
 */
 /*
ISR (USART0_RX_vect)
{
	receiveComplete0();
}*/

ISR (USART1_RX_vect)
{
	receiveComplete1();
}
