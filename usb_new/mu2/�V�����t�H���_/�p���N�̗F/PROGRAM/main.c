#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart/uart.h"
#include "mu2/mu2_common.h"
#include "mu2/mu2_control.h"
#include "nunchuk/rc_nunchuk.h"
#include "classic/rc_classic.h"

#define SWITCH	(PINB & 0x0F)

int nunchuk(int,unsigned char *);
int classic(int,unsigned char *);
void setup();
void set_adc();
int mode();
int wii_nunchuk();

int main(void)
{
	int len;

	setup();												//Port Setting
	set_adc();												//ADconvert Setting

	if( wii_nunchuk())	len=NUNCHUK_DATA_LENGTH;
	if(!wii_nunchuk())	len=CLASSIC_DATA_LENGTH;

	unsigned char TxDataBuf[len*5];							//uart�̑��M�p�o�b�t�@�����Ɉꎞ�I�ɑ��M����f�[�^��~����
	unsigned char analog_data[5];	
	unsigned char Group[4] 		 = {0x21,0x9A,0x44,0x80};	//GroupID List
	unsigned char Channel[4][4]  ={{0x07,0x08,0x09,0x0A},	//Channel List
							 	   {0x0C,0x0D,0x0E,0x0F},
								   {0x11,0x12,0x13,0x14},
								   {0x16,0x17,0x18,0x19}};	
	uart_init( UART_RE|UART_TE, 19200);						//��M����|���M����|���M�f�[�^�󊄂荞�݋��C�{�[���[�g
	uart_setbuffer(TxDataBuf,len*5);						//���M�p�o�b�t�@�̐ݒ�
  
    MU2_SetRxHandler(uart_getchar);							//uart��1byte��M�֐����Z�b�g
    MU2_SetTxHandler(uart_putchar);							//uart��1byte���M�\��֐����Z�b�g

	MU2_SetGroupID(Group[SWITCH>>2]);						//Set GroupID
	MU2_SetChannel(Channel[SWITCH>>2][SWITCH & 0x03]);		//Set Channel
	
	if(wii_nunchuk()){										//Nunchuk Mode
		PORTB &=~_BV(PC1);									//LED ON
		nunchuk(mode(),analog_data);						
	}
	if(!(wii_nunchuk())){									//Classic Mode
		PORTB &=~_BV(PC2);									//LED ON
		classic(mode(),analog_data);
	}
	return 0;
}
