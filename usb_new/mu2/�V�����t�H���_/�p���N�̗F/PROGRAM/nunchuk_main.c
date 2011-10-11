#include "uart/uart.h"
#include "mu2/mu2_common.h"
#include "mu2/mu2_data.h"
#include "nunchuk/nunchuk.h"
#include "nunchuk/rc_nunchuk.h"

void set_controller();
void get_value(unsigned char *);
void get_nunchuk_analog(const union wii_nunchuk *,unsigned char *);
void MakeNunchukRCData(union nunchuk_data *rcdata,const union wii_nunchuk *data,unsigned char *analog_data);
void wait();
void battery_check();

int nunchuk(int mode,unsigned char *analog_data)
{
	int i;	
	
	union wii_nunchuk data;										
	union nunchuk_data rcdata;										
	
	while(1){
		set_controller();									//�R���g���[���[������				
		get_value( data.buf );								//�R���g���[���[����l�擾(I2C)
		get_nunchuk_analog(&data,analog_data);				//�A�i���O�X�e�B�b�N�Ɖ����x�Z���T�̒l���擾
		MakeNunchukRCData(&rcdata,&data,analog_data);		//rcdata�̃o�b�t�@�ɒl������
    	if(mode==0)	MU2_SendData(rcdata.buf,NUNCHUK_DATA_LENGTH);//MU2�Ńf�[�^���M
		if(mode==1)	for(i=0;i<NUNCHUK_DATA_LENGTH;i++){
						MU2_TxChar(rcdata.buf[i]);
						}
		wait(45);
		battery_check();
	}
    return 0;
}
