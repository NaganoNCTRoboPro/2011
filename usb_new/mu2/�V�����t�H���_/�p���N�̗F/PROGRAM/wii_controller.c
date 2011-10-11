#include <util/twi.h>

#define I2C_START 	(TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN))
#define I2C_END	  	(TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN))

void wait(uint16_t w)							//about 1ms scale 
{
	while(w--){
		volatile uint16_t i=160;
		while(i--);
	}
}

void error()									//Processing Some Errors
{
	while(1){									//LED flashing
		PORTC |= _BV(PC1);						
		PORTC |= _BV(PC2);
		wait(750);
		PORTC &=~_BV(PC1);
		PORTC &=~_BV(PC2); 
		wait(750);
	}
}

void setup()									//Init MicroComputer
{
	DDRB  = 0x00;								//Set  Input  for   PortB
	PORTB = 0xFF;								//Set  0xFF   for   PortB�@�@�@
	DDRC  = 0xFF;								//Set  Outpur for   PortC
	PORTC = 0xFF;								//Set  0xFF   for   PortC
	DDRD  = 0x00;								//Set  Input  for   PortD							
	PORTD = 0xFF;								//Set  0xFF   for   PortD
	TWBR  = 3;									//Set  I2C    about 363kHz
	TWSR  = 0x00;								//Init TWSR
}

void set_adc()
{
	DDRC =  0xFE;							//PC0�ȊO��PORTC���o��
	PORTC = 0xFE;							//PC0�ȊO��PORTC��1�ɂ���
	ADMUX = 0x60;							// Basic:AVcc,ADConvertPort:PC3
	ADCSRA= 0x86;							// ADConvert Permission,Clock:125kHz
	DIDR0 = 0x0f;							// Input digital prohibition for PC3
}

void set_controller()							//Setting Wii Controller
{												//Controller Address W:0xA4,R:0xA5
	I2C_START;									//Send I2C start condition

	while(!(TWCR & _BV(TWINT)));				//Wait while TWINT flag = 1
	if((TWSR & 0xF8) != TW_START)		error();//Check TWSR
	TWDR = 0xA4;								//Set Send Data Slave Address+W
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//Send Start(TWINT flag = 0)
	while(!(TWCR & _BV(TWINT))) ;				//Wait while TWINT flag = 1
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
		if(TW_STATUS!=TW_MR_DATA_ACK)error();	//ACK���Ԃ��Ă��Ȃ�������error
		Re_Data[i] = (TWDR^0x17)+0x17;			//wii�Ή��f�[�^�ɕϊ�����Re_Data�Ɋi�[
		}
	I2C_END;									//�I�������𑗂�	
}
//select MU2 or Serial
//mode�@0���MU2�@1���serial
int mode()
{
	int mode;
	if( PD4)	mode=0;
	if(!PD4)	mode=1;
	return mode;
}

//select Nunchuk or Classic
//1=nunchuk  0=classic
int wii_nunchuk()
{
	int nunchuk;
	if( PD5)	nunchuk=1;
	if(!PD5)	nunchuk=0;
	return nunchuk;
}

//��{�v�Z���@ADC=(Vin*1024)/AVcc
//ADC��10bit��ADCH�͏��8bit
//�����Vin�͓d���d���̔���(��R�ŕ���)
//�@�@AVcc�̓��M�����[�V�������3.3V���g�p
//����͓d����4V�����̂Ƃ��_�ł�����������
//Vin=2.0�Ōv�Z
//	ADC=(2*1024)/3.3=620=0b1001101100
//	�����ADCH=0b10011011=0x9B
// �ȏ��荡���0x9b�����ɂȂ�����LED�_��

void battery_check(){
	ADCSRA |= 0x50;					// start AD convert
	while(ADIF==0);					// Waiting while converting
	if(ADCH < 0x9B){				// low battery(under 4V)
		wait(ADCH*10);
		PORTC |= _BV(PC1);			// LED flashing	
		PORTC &=~_BV(PC2);
		wait(ADCH*10);
		PORTC &=~_BV(PC1);
		PORTC |= _BV(PC2);
	}
	ADCSRA |= 0x10;
}
