#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart/uart.h"
#include "mu2/mu2_common.h"
#include "mu2/mu2_control.h"
#include "mu2/mu2_data.h"
#include "nunchuk/nunchuk.h"
#include "nunchuk/rc_nunchuk.h"

#define SWITCH	((PINC^0xff) & 0x0F)

void setup();
void set_controller();
void get_value(unsigned char *);
void get_analog(const union wii_nunchuk *,unsigned char *);
void MakeRCData(union controller_data *rcdata,const union wii_nunchuk *data,unsigned char *);
void wait();

int main(void)
{
	setup();

	unsigned char analog_data[4];
	unsigned char TxDataBuf[RC_DATA_LENGTH*5];				//uart�̑��M�p�o�b�t�@�����Ɉꎞ�I�ɑ��M����f�[�^��~����
	unsigned char Group[4] 		 = {0x21,0x9A,0x44,0x80};	//�O���[�v���X�g
	unsigned char Channel[4][4]  ={{0x07,0x08,0x09,0x0A},	//�`�����l�����X�g
							 	   {0x0C,0x0D,0x0E,0x0F},
								   {0x11,0x12,0x13,0x14},
								   {0x16,0x17,0x18,0x19}};
								   	
	union wii_nunchuk data;									//wii�R���g���[���[����󂯂Ƃ�f�[�^
	union controller_data rcdata;							//MU2�ɑ���f�[�^

    uart_init( UART_RE|UART_TE, 19200);						//��M����|���M����|���M�f�[�^�󊄂荞�݋��C�{�[���[�g
    uart_setbuffer(TxDataBuf,RC_DATA_LENGTH*5);				//���M�p�o�b�t�@�̐ݒ�
    
    MU2_SetRxHandler(uart_getchar);							//uart��1byte��M�֐����Z�b�g
    MU2_SetTxHandler(uart_putchar);							//uart��1byte���M�\��֐����Z�b�g������uart_putchar�� 
															//�����Ɗ��荞�݂ł͂Ȃ�wait�`���ɂȂ�			  	
	MU2_SetGroupID(Group[SWITCH>>2]);						//�O���[�v�ݒ�
	MU2_SetChannel(Channel[SWITCH>>2][SWITCH & 0x03]);		//�`�����l���ݒ�

	while(1){
		set_controller();									//�R���g���[���[������				
		get_value( data.buf );								//�R���g���[���[����l�擾(I2C)
		get_analog(&data,analog_data);						//�A�i���O�X�e�B�b�N�Ɖ����x�Z���T�̒l���擾
		MakeRCData(&rcdata,&data,analog_data);							//rcdata�̃o�b�t�@�ɒl������
		MU2_SendData(rcdata.buf,RC_DATA_LENGTH);			//MU2�Ńf�[�^���M
		wait(45);											//wait(45ms)
	}
    return 0;
}
