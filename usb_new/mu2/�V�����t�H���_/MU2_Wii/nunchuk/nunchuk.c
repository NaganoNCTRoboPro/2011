#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/twi.h>
#include <string.h>
#include <stdlib.h>
#include "nunchuk.h"
#include "rc_nunchuk.h"

#define I2C_START 	(TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN))
#define I2C_END	  	(TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN))

void wait(uint16_t w)	//��Pms�P��
{
	while(w--){
		volatile uint16_t i=160;
		while(i--);
	}
}

void error()					//�G���[����
{
	while(1){
		PORTD |= _BV(PD7);		//�d��LED��_�ł�����
		wait(200);
		PORTD &=~_BV(PD7);
		wait(200);
	}
}

void setup()									//�}�C�R���̏�����
{
	DDRB  = 0x00;								//�|�[�gB����͂ɃZ�b�g 
	PORTB = 0xFF;								//�|�[�gB�ɂ��ׂ�1���Z�b�g�@�@�@
	DDRC  = 0x00;								//�|�[�gC����͂ɃZ�b�g
	PORTC = 0xFF;								//�|�[�gC�ɂ��ׂ�1���Z�b�g
	DDRD  = 0xFF;								//�|�[�gD���o�͂ɃZ�b�g
	PORTD = 0xFF;								//�|�[�gD�ɂ��ׂ�1���Z�b�g
	TWBR  = 3;									//I2C�̐ݒ�@��363kHz
	TWSR  = 0x00;								//TWSR�̏�����
	PORTD &=~_BV(PD7);							//�d��LED ON
}

void set_controller()							//wii�R���g���[���[�̏����ݒ�
{												//�R���g���[���[�̃A�h���XW:0xA4,R:0xA5
	I2C_START;									//I2C�̊J�n�����𑗂�

	while(!(TWCR & _BV(TWINT)));				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_START)		error();//��ԃR�[�h(TWSR)�𒲂ׂ�

	TWDR = 0xA4;								//���M�f�[�^�ɃX���[�u�A�h���X+W��ݒ�
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINT�t���O��|���đ��M�J�n
	
	while(!(TWCR & _BV(TWINT))) ;				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACK���Ԃ��Ă��Ȃ�������error

	TWDR = 0x40;								//���M�f�[�^��ݒ�    
	TWCR = _BV(TWINT) | _BV(TWEN) ;
	
	while(!(TWCR & _BV(TWINT))) ;				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//ACK���Ԃ��Ă��Ȃ�������error
	
	TWDR = 0x00;								//���M�f�[�^��ݒ�    
	TWCR = _BV(TWINT) | _BV(TWEN) ;
	while(!(TWCR & _BV(TWINT))) ;				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//ACK���Ԃ��Ă��Ȃ�������error

	I2C_END;									//�I�������𑗂�
}

void get_value(unsigned char *Re_Data)
{
	int i;

	wait(2);									//wait(2ms)

	I2C_START;									//�J�n�����𑗂�

	while(!(TWCR & _BV(TWINT)));				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_START)		error();//��ԃR�[�h(TWSR)�𒲂ׂ�

	TWDR = 0xA4;								//���M�f�[�^�ɃX���[�u�A�h���X+Write��ݒ�
	TWCR = _BV(TWINT) | _BV(TWEN);				//TWINT�t���O��|���đ��M�J�n
	
	while(!(TWCR & _BV(TWINT)));				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACK���Ԃ��Ă��Ȃ�������error

	TWDR = 0x00;								//���M�f�[�^��ݒ�    
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINT�t���O��|���đ��M�J�n
	
	while(!(TWCR & _BV(TWINT))) ;				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//ACK���Ԃ��Ă��Ȃ�������error

	I2C_END;									//�I�������𑗂�

	wait(1);									//wait(1ms)

	I2C_START;									//�J�n�����𑗂�

	while(!(TWCR & _BV(TWINT)));				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_START)		error();//��ԃR�[�h(TWSR)�𒲂ׂ�

	TWDR = 0xA5;								//���M�f�[�^�ɃX���[�u�A�h���X+Read��ݒ�
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINT�t���O��|���đ��M�J�n
	
	while(!(TWCR & _BV(TWINT))) ;				//TWINT�t���O�����܂őҋ@
	if((TWSR & 0xF8) != TW_MR_SLA_ACK)	error();//ACK���Ԃ��Ă��Ȃ�������error

	for(i=0; i<6; i++){
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);	//data��M+ACK�\��
		while(!(TWCR & _BV(TWINT))) ;			//TWINT�t���O�����܂őҋ@
		if (TW_STATUS!=TW_MR_DATA_ACK)	error();//ACK���Ԃ��Ă��Ȃ�������error
		Re_Data[i] = (TWDR^0x17)+0x17;			//wii�Ή��f�[�^�ɕϊ�����Re_Data�Ɋi�[
	}
	I2C_END;									//�I�������𑗂�	
}

void MakeRCData(union controller_data *rcdata,const union wii_nunchuk *data, unsigned char *analog_data)
{
	// Button
	rcdata->detail.Button.C				= data->detail.Button.C ^ 1;	//�{�^��C
	rcdata->detail.Button.Z				= data->detail.Button.Z ^ 1;	//�{�^��Z
	
	//Acceleration
	rcdata->detail.Acceleration.X		= analog_data[2] & 0x07;		//�����xX
	rcdata->detail.Acceleration.Y		= analog_data[3] & 0x07;		//�����xY
	rcdata->detail.Acceleration.Z		= analog_data[4] & 0x03;		//�����xZ

	// Analogue
	rcdata->detail.Analog.X		= analog_data[0] & 0x0f;				//�A�i���O�X�e�B�b�NX��
	rcdata->detail.Analog.Y		= (14-analog_data[1]) & 0x0f;			//�A�i���O�X�e�B�b�NY��
}

void get_analog( const union wii_nunchuk *data , unsigned char *analog_data)
{
	int i;

//analog_data�ɃA�i���O�X�e�B�b�N�Ɖ����x�̒l���擾�������̂�����	
	analog_data[0] = (unsigned char)GetAnalogueX( data );				//�A�i���O�X�e�B�b�NX��
	analog_data[1] = (unsigned char)GetAnalogueY( data );				//�A�i���O�X�e�B�b�NY��
	analog_data[2] = (unsigned char)GetXacceleration( data )>>4;		//�����xX��
	analog_data[3] = (unsigned char)GetYacceleration( data )>>4;		//�����xY��
	analog_data[4] = (unsigned char)GetZacceleration( data )>>4;		//�����xZ��

//�A�i���O�X�e�B�b�N
	for(i=0; i<=1; i++){
		     if(analog_data[i]<= 65) analog_data[i] = 0x00;
		else if(analog_data[i]<= 75) analog_data[i] = 0x01;
		else if(analog_data[i]<= 85) analog_data[i] = 0x02;
		else if(analog_data[i]<= 95) analog_data[i] = 0x03;
		else if(analog_data[i]<=105) analog_data[i] = 0x04;
		else if(analog_data[i]<=115) analog_data[i] = 0x05;
		else if(analog_data[i]<=125) analog_data[i] = 0x06;
		else if(analog_data[i]<=135) analog_data[i] = 0x07;
		else if(analog_data[i]<=190) analog_data[i] = 0x08;
		else if(analog_data[i]<=195) analog_data[i] = 0x09;
		else if(analog_data[i]<=200) analog_data[i] = 0x0A;
		else if(analog_data[i]<=205) analog_data[i] = 0x0B;
		else if(analog_data[i]<=210) analog_data[i] = 0x0C;
		else if(analog_data[i]<=215) analog_data[i] = 0x0D;
		else if(analog_data[i]>=218) analog_data[i] = 0x0E;
	}
// �����x
	//X,Y��
	for(i=2;i<=3;i++){
			 if(analog_data[i]<   4) analog_data[i] = 0x00;
		else if(analog_data[i]<=  5) analog_data[i] = 0x01;
		else if(analog_data[i]<=  6) analog_data[i] = 0x02;
		else if(analog_data[i]<=  7) analog_data[i] = 0x03;
		else if(analog_data[i]<=  8) analog_data[i] = 0x04;
		else if(analog_data[i]<=  9) analog_data[i] = 0x05;
		else if(analog_data[i]<= 10) analog_data[i] = 0x06;
		else if(analog_data[i]>  10) analog_data[i] = 0x07;
	}
	//Z��
	for(i=4;i==4;i++){
			 if(analog_data[i]<   5) analog_data[i] = 0x00;
		else if(analog_data[i]<=  7) analog_data[i] = 0x01;
		else if(analog_data[i]<   9) analog_data[i] = 0x02;
		else if(analog_data[i]>=  9) analog_data[i] = 0x03;
	}
}
