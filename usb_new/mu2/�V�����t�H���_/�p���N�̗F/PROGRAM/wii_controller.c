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
	PORTB = 0xFF;								//Set  0xFF   for   PortB　　　
	DDRC  = 0xFF;								//Set  Outpur for   PortC
	PORTC = 0xFF;								//Set  0xFF   for   PortC
	DDRD  = 0x00;								//Set  Input  for   PortD							
	PORTD = 0xFF;								//Set  0xFF   for   PortD
	TWBR  = 3;									//Set  I2C    about 363kHz
	TWSR  = 0x00;								//Init TWSR
}

void set_adc()
{
	DDRC =  0xFE;							//PC0以外のPORTCを出力
	PORTC = 0xFE;							//PC0以外のPORTCを1にする
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
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	error();//ACKが返ってこなかったらerror
	TWDR = 0x40;								//送信データを設定    
	TWCR = _BV(TWINT) | _BV(TWEN) ;
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//ACKが返ってこなかったらerror
	TWDR = 0x00;								//送信データを設定    
	TWCR = _BV(TWINT) | _BV(TWEN) ;
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)	error();//ACKが返ってこなかったらerror
	I2C_END;									//終了条件を送る
}

void get_value(unsigned char *Re_Data)
{
	int i;

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
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる
	TWDR = 0xA5;								//送信データにスレーブアドレス+Readを設定
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINTフラグを倒して送信開始
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_MR_SLA_ACK)	error();//ACKが返ってこなかったらerror
	for(i=0; i<6; i++){
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);	//data受信+ACK予約
		while(!(TWCR & _BV(TWINT))) ;			//TWINTフラグが立つまで待機
		if(TW_STATUS!=TW_MR_DATA_ACK)error();	//ACKが返ってこなかったらerror
		Re_Data[i] = (TWDR^0x17)+0x17;			//wii対応データに変換してRe_Dataに格納
		}
	I2C_END;									//終了条件を送る	
}
//select MU2 or Serial
//mode　0･･･MU2　1･･･serial
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

//基本計算式　ADC=(Vin*1024)/AVcc
//ADCは10bitでADCHは上位8bit
//今回はVinは電源電圧の半分(抵抗で分圧)
//　　AVccはレギュレーション後の3.3Vを使用
//今回は電源が4V未満のとき点滅させたいため
//Vin=2.0で計算
//	ADC=(2*1024)/3.3=620=0b1001101100
//	よってADCH=0b10011011=0x9B
// 以上より今回は0x9b未満になったらLED点滅

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
