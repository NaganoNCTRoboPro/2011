#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/twi.h>
#include <string.h>
#include <stdlib.h>
#include "classic.h"
#include "rc_classic.h"

#define I2C_START 	(TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN))
#define I2C_END	  	(TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN))

void MakeRCData(union controller_data *rcdata, const union wii_classic *data, unsigned char *analog_data)
{
	// Button
	rcdata->detail.Button.LEFT		= data->detail.Button.Left	^ 1;
	rcdata->detail.Button.DOWN		= data->detail.Button.Down	^ 1;
	rcdata->detail.Button.RIGHT		= data->detail.Button.Right	^ 1;
	rcdata->detail.Button.UP		= data->detail.Button.Up	^ 1;
	rcdata->detail.Button.Y			= data->detail.Button.y		^ 1;
	rcdata->detail.Button.B			= data->detail.Button.b		^ 1;
	rcdata->detail.Button.A			= data->detail.Button.a		^ 1;
	rcdata->detail.Button.X			= data->detail.Button.x		^ 1;
	rcdata->detail.Button.R			= GetAnalogueRS( data )		>>3;
	rcdata->detail.Button.L			= GetAnalogueLS( data )		>>3;
	rcdata->detail.Button.ZR		= data->detail.Button.ZR	^ 1;
	rcdata->detail.Button.ZL		= data->detail.Button.ZL	^ 1;
	rcdata->detail.Button.START		= data->detail.Button.Start ^ 1;
	rcdata->detail.Button.SELECT	= data->detail.Button.Select^ 1;
	// Analogue
	rcdata->detail.AnalogL.X		= (   analog_data[0]) & 0x0f;
	rcdata->detail.AnalogL.Y		= (14-analog_data[1]) & 0x0f;
	rcdata->detail.AnalogR.X		= (   analog_data[2]) & 0x0f;	
	rcdata->detail.AnalogR.Y		= (14-analog_data[3]) & 0x0f;
}

void wait(uint16_t w)	//��Pms�P��
{
	while(w--){
		volatile uint16_t i=160;
		while(i--);
	}
}

void error()				//�G���[����
{
	while(1){
		PORTD |= _BV(PD7);	//�d��LED��_�ł�����
		wait(200);
		PORTD &=~_BV(PD7);
		wait(200);
		PORTD |= _BV(PD7);
	}
}

void setup()									//�}�C�R���̏�����
{
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

void get_analog( const union wii_classic *data, unsigned char *analog_data )
{
	int i;	
	
	//�e�A�i���O�X�e�B�b�N�̒l��analog_data[]�Ɋi�[
	analog_data[0] = (unsigned char)GetAnalogueLeftX( data );
	analog_data[1] = (unsigned char)GetAnalogueLeftY( data );
	analog_data[2] = (unsigned char)GetAnalogueRightX( data );
	analog_data[3] = (unsigned char)GetAnalogueRightY( data );

	//�A�i���O�X�e�B�b�N�̒l�̏���(��)
	for(i=0; i<2; i++){
			 if(analog_data[i]<=10)	analog_data[i] = 0x00;
		else if(analog_data[i]<=11)	analog_data[i] = 0x01;
		else if(analog_data[i]<=13) analog_data[i] = 0x02;
		else if(analog_data[i]<=17) analog_data[i] = 0x03;
		else if(analog_data[i]<=21) analog_data[i] = 0x04;
		else if(analog_data[i]<=25) analog_data[i] = 0x05;
		else if(analog_data[i]<=29) analog_data[i] = 0x06;
		else if(analog_data[i]<=34) analog_data[i] = 0x07;
		else if(analog_data[i]<=38) analog_data[i] = 0x08;
		else if(analog_data[i]<=42) analog_data[i] = 0x09;
		else if(analog_data[i]<=46) analog_data[i] = 0x0A;
		else if(analog_data[i]<=50) analog_data[i] = 0x0B;
		else if(analog_data[i]<=53) analog_data[i] = 0x0C;
		else if(analog_data[i]<=55) analog_data[i] = 0x0D;
		else if(analog_data[i]> 58) analog_data[i] = 0x0E;	
	}
	
	//�A�i���O�X�e�B�b�N�̒l�̏���(�E)
	for(i=2; i<5; i++){
			 if(analog_data[i]<=5)  analog_data[i] = 0x00;
		else if(analog_data[i]<=6)  analog_data[i] = 0x01;
		else if(analog_data[i]<=7)  analog_data[i] = 0x02;
		else if(analog_data[i]<=8)  analog_data[i] = 0x03;
		else if(analog_data[i]<=10) analog_data[i] = 0x04;
		else if(analog_data[i]<=12) analog_data[i] = 0x05;
		else if(analog_data[i]<=14) analog_data[i] = 0x06;
		else if(analog_data[i]<=16) analog_data[i] = 0x07;
		else if(analog_data[i]<=18) analog_data[i] = 0x08;
		else if(analog_data[i]<=20) analog_data[i] = 0x09;
		else if(analog_data[i]<=22) analog_data[i] = 0x0A;
		else if(analog_data[i]<=23) analog_data[i] = 0x0B;
		else if(analog_data[i]<=24) analog_data[i] = 0x0C;
		else if(analog_data[i]<=25) analog_data[i] = 0x0D;
		else if(analog_data[i]>=26) analog_data[i] = 0x0E;
	}

	
}

