#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "mu2/mu2.h"


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
		wdt_enable(WDTO_15MS);
	}
}

void setup()									//Init MicroComputer
{
	DDRB  = 0xC0;								//Set PortB(Input-5,4,3,2,1,0 Output-7.6)
	PORTB = 0xFF;								//Set PortB(1,1,1,1,1,1,1,1)　　　
	DDRC  = 0xFE;								//Set PortC(Input-0 Output-7.6.5.4.3.2.1)
	PORTC = 0xFE;								//Set PortC(1,1,1,1,1,1,1,0)
	DDRD  = 0xFC;								//Set PortD(Input-3,2 Output-7,6,5,4,1,0)							
	PORTD = 0xFF;								//Set PortD(1,1,1,1,1,1,1,1)
	TWBR  = 3;									//Set  I2C    about 363kHz
	TWSR  = 0x00;								//Init TWSR
	PORTC &=~_BV(PC3);							//SETUP SUCCESS!!
}

void set_adc()
{
	ADMUX = 0x60;								//Basic:AVcc,ADConvertPort:PC3
	ADCSRA= 0x86;								//ADConvert Permission,Clock:125kHz
	DIDR0 = 0x0f;								//Input digital prohibition for PC3
}

void set_controller()							//Setting Wii Controller
{												//Controller Address W:0xA4,R:0xA5
	I2C_START;									//Send I2C start condition
	while(!(TWCR & _BV(TWINT)));				//Wait while TWINT flag = 1
	if((TWSR & 0xF8) != TW_START)		error();//Check TWSR
	TWDR = 0xA4;								//Set Send Data Slave Address+W
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//Send Start(TWINT flag = 0)
	while(!(TWCR & _BV(TWINT))) ;				//Wait while TWINT-flag = 1
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//if not return ACK,it is error.
	TWDR = 0x40;								//Setting Send Data   
	TWCR = _BV(TWINT) | _BV(TWEN) ;
	while(!(TWCR & _BV(TWINT))) ;				//Wait while TWINT-flag = 1
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//if not return ACK,it is error.
	TWDR = 0x00;								//Setting send data
	TWCR = _BV(TWINT) | _BV(TWEN) ;
	while(!(TWCR & _BV(TWINT))) ;				//Wait while TWINT-flag = 1
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//if not return ACK,it is error.
	I2C_END;									//Send I2C end condition
}

void get_value(unsigned char *Re_Data)
{
	int i;
	wdt_enable(WDTO_500MS);
	wait(2);									//wait(2ms)
	I2C_START;									//開始条件を送る
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる
	TWDR = 0xA4;								//送信データにスレーブアドレス+Writeを設定
	TWCR = _BV(TWINT) | _BV(TWEN);				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACKが返ってこなかったらerror
	TWDR = 0x00;								//送信データを設定    
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//ACKが返ってこなかったらerror
	I2C_END;									//終了条件を送る
	wait(1);									//wait(1ms)
	
	I2C_START;									//開始条件を送る
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)	error();//状態コード(TWSR)を調べる
	TWDR = 0xA5;								//送信データにスレーブアドレス+Readを設定
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MR_SLA_ACK)	error();//ACKが返ってこなかったらerror
	for(i=0; i<6; i++){
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);	//data受信+ACK予約
		while(!(TWCR & _BV(TWINT))) ;			//TWINTフラグが立つまで待機
		if(TW_STATUS!=TW_MR_DATA_ACK)   error();//ACKが返ってこなかったらerror
		Re_Data[i] = (TWDR^0x17)+0x17;			//wii対応データに変換してRe_Dataに格納
		}
	I2C_END;									//終了条件を送る	
	wdt_disable();
}

//select MU2 or Serial
//mode　0･･･MU2　1･･･serial
int mode()
{
	int mode;	
	if(bit_is_set(PIND,PD2))	mode=0;
	if(bit_is_clear(PIND,PD2))	mode=1;
	return mode;
}

void make_key()									//暗号化鍵の送信/作成
{
	int i;
	unsigned char data1[7]={0x40,0xA4,0xDD,0x6D,0x0C,0x2F,0x7E};
	unsigned char data2[7]={0x46,0xF2,0x83,0xD4,0xE0,0xFD,0x26};
	unsigned char data3[5]={0x4C,0xE3,0xB3,0x98,0x67};
	
	wait(2);									//wait(2ms)	
	I2C_START;									//開始条件を送る
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる
	TWDR = 0xA4;								//送信データにスレーブアドレス+Writeを設定
	TWCR = _BV(TWINT) | _BV(TWEN);				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACKが返ってこなかったらerror
	for(i=0;i<7;i++){
		TWDR = data1[i];						//送信データを設定    
		TWCR = _BV(TWINT) | _BV(TWEN);			//TWINTフラグを倒して送信開始
		while(!(TWCR & _BV(TWINT)));			//TWINTフラグが立つまで待機
		if((TWSR & 0xF8)!=TW_MT_DATA_ACK)error();//ACKが返ってこなかったらerror
	}
	I2C_END;									//終了条件を送る
	
	wait(2);									//wait(2ms)	
	I2C_START;									//開始条件を送る
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる
	TWDR = 0xA4;								//送信データにスレーブアドレス+Writeを設定
	TWCR = _BV(TWINT) | _BV(TWEN);				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACKが返ってこなかったらerror
	for(i=0;i<7;i++){
		TWDR = data2[i];						//送信データを設定    
		TWCR = _BV(TWINT) | _BV(TWEN);			//TWINTフラグを倒して送信開始
		while(!(TWCR & _BV(TWINT)));			//TWINTフラグが立つまで待機
		if((TWSR & 0xF8)!=TW_MT_DATA_ACK)error();//ACKが返ってこなかったらerror
	}
	I2C_END;									//終了条件を送る
	
	wait(2);									//wait(2ms)	
	I2C_START;									//開始条件を送る
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる
	TWDR = 0xA4;								//送信データにスレーブアドレス+Writeを設定
	TWCR = _BV(TWINT) | _BV(TWEN);				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACKが返ってこなかったらerror
	for(i=0;i<5;i++){
		TWDR = data3[i];						//送信データを設定    
		TWCR = _BV(TWINT) | _BV(TWEN);			//TWINTフラグを倒して送信開始
		while(!(TWCR & _BV(TWINT)));			//TWINTフラグが立つまで待機
		if((TWSR & 0xF8)!=TW_MT_DATA_ACK)error();//ACKが返ってこなかったらerror
	}
	I2C_END;									//終了条件を送る
}

//select Nunchuk or Classic
//1=nunchuk  0=classic
int controller_select()							//コントローラーの選択
{
	int i,ctrl;
	unsigned char data[2];
	
	make_key();									//暗号化鍵

	wait(2);									//wait(2ms)
	I2C_START;									//開始条件を送る
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる
	TWDR = 0xA4;								//送信データにスレーブアドレス+Writeを設定
	TWCR = _BV(TWINT) | _BV(TWEN);				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACKが返ってこなかったらerror
	TWDR = 0xFE;								//送信データを設定    
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//ACKが返ってこなかったらerror
	I2C_END;									//終了条件を送る
	wait(1);									//wait(1ms)

	I2C_START;									//開始条件を送る
	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる
	TWDR = 0xA5;								//送信データにスレーブアドレス+Readを設定
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MR_SLA_ACK)	error();//ACKが返ってこなかったらerror
	for(i=0;i<2;i++){
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);	//data受信+ACK予約
		while(!(TWCR & _BV(TWINT))) ;			//TWINTフラグが立つまで待機
		if(TW_STATUS!=TW_MR_DATA_ACK)   error();//ACKが返ってこなかったらerror
		data[i] = TWDR;							//TWDRをdataに格納
	}
	I2C_END;									//終了条件を送る	

	if		(data[1]<=0x80)	ctrl=0;				//クラシック
	else if	(data[1]>=0x81)	ctrl=1;				//ヌンチャク
	else	error();							//エラー

//	if((data[0]==0x00)&&(data[1]==0x00))		ctrl =1;
//	if((data[0]==0x01)&&(data[1]==0x01))	ctrl =0;

	return ctrl;
}

//ADC=(Vin*1024)/AVcc
void battery_check()
{
//caution!!
	ADCSRA |= 0x50;						// start AD convert
	while(ADIF==0);						// Waiting while converting
	if(ADCH < 0xA2){					// low battery
		PORTD &= 0x0F;	  
		PORTB &= 0x3F;	 
		wait(ADCH*10);
		PORTD |= 0xF0;
		PORTB |= 0xC0;	
		wait(ADCH*10);
	}

//states display
	PORTD &= 0x0F;						//LED ON  
	PORTB &= 0x3F;						//LED ON  
	if(ADCH < 0xC2)	PORTD |= _BV(PD7);	//LED OFF
	if(ADCH < 0xBB) PORTD |= _BV(PD6);	//LED OFF
	if(ADCH < 0xB5) PORTD |= _BV(PD5);  //LED OFF
	if(ADCH < 0xAF) PORTB |= _BV(PB7);	//LED OFF
	if(ADCH < 0xAA) PORTB |= _BV(PB6);	//LED OFF

	ADCSRA |= 0x10;					//ADCSRA INIT
}

void battery_states()
{
	ADCSRA |= 0x50;					// start AD convert
	while(ADIF==0);					// Waiting while converting
	PORTD &=~_BV(PD4);
	wait(250);
	if(ADCH > 0xAA){
		PORTB &=~_BV(PB6);
		wait(250);
	}
	if(ADCH > 0xAF){
		PORTB &=~_BV(PB7);
		wait(250);
	}
	if(ADCH > 0xB5){
		PORTD &=~_BV(PD5);
		wait(250);
	}
	if(ADCH > 0xBB){
		PORTD &=~_BV(PD6);
		wait(250);
	}
	if(ADCH > 0xC2){
		PORTD &=~_BV(PD7);
		wait(250);	
	}
	ADCSRA|= 0x10;					//ADCSRA INIT
}

