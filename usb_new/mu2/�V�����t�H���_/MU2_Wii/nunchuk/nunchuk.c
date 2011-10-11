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

void wait(uint16_t w)	//約１ms単位
{
	while(w--){
		volatile uint16_t i=160;
		while(i--);
	}
}

void error()					//エラー処理
{
	while(1){
		PORTD |= _BV(PD7);		//電源LEDを点滅させる
		wait(200);
		PORTD &=~_BV(PD7);
		wait(200);
	}
}

void setup()									//マイコンの初期化
{
	DDRB  = 0x00;								//ポートBを入力にセット 
	PORTB = 0xFF;								//ポートBにすべて1をセット　　　
	DDRC  = 0x00;								//ポートCを入力にセット
	PORTC = 0xFF;								//ポートCにすべて1をセット
	DDRD  = 0xFF;								//ポートDを出力にセット
	PORTD = 0xFF;								//ポートDにすべて1をセット
	TWBR  = 3;									//I2Cの設定　約363kHz
	TWSR  = 0x00;								//TWSRの初期化
	PORTD &=~_BV(PD7);							//電源LED ON
}

void set_controller()							//wiiコントローラーの初期設定
{												//コントローラーのアドレスW:0xA4,R:0xA5
	I2C_START;									//I2Cの開始条件を送る

	while(!(TWCR & _BV(TWINT)));				//TWINTフラグが立つまで待機
	if((TWSR & 0xF8) != TW_START)		error();//状態コード(TWSR)を調べる

	TWDR = 0xA4;								//送信データにスレーブアドレス+Wを設定
	TWCR = _BV(TWINT) | _BV(TWEN) ;				//TWINTフラグを倒して送信開始
	
	while(!(TWCR & _BV(TWINT))) ;				//TWINTフラグが立つまで待機
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
		if (TW_STATUS!=TW_MR_DATA_ACK)	error();//ACKが返ってこなかったらerror
		Re_Data[i] = (TWDR^0x17)+0x17;			//wii対応データに変換してRe_Dataに格納
	}
	I2C_END;									//終了条件を送る	
}

void MakeRCData(union controller_data *rcdata,const union wii_nunchuk *data, unsigned char *analog_data)
{
	// Button
	rcdata->detail.Button.C				= data->detail.Button.C ^ 1;	//ボタンC
	rcdata->detail.Button.Z				= data->detail.Button.Z ^ 1;	//ボタンZ
	
	//Acceleration
	rcdata->detail.Acceleration.X		= analog_data[2] & 0x07;		//加速度X
	rcdata->detail.Acceleration.Y		= analog_data[3] & 0x07;		//加速度Y
	rcdata->detail.Acceleration.Z		= analog_data[4] & 0x03;		//加速度Z

	// Analogue
	rcdata->detail.Analog.X		= analog_data[0] & 0x0f;				//アナログスティックX軸
	rcdata->detail.Analog.Y		= (14-analog_data[1]) & 0x0f;			//アナログスティックY軸
}

void get_analog( const union wii_nunchuk *data , unsigned char *analog_data)
{
	int i;

//analog_dataにアナログスティックと加速度の値を取得したものを入れる	
	analog_data[0] = (unsigned char)GetAnalogueX( data );				//アナログスティックX軸
	analog_data[1] = (unsigned char)GetAnalogueY( data );				//アナログスティックY軸
	analog_data[2] = (unsigned char)GetXacceleration( data )>>4;		//加速度X軸
	analog_data[3] = (unsigned char)GetYacceleration( data )>>4;		//加速度Y軸
	analog_data[4] = (unsigned char)GetZacceleration( data )>>4;		//加速度Z軸

//アナログスティック
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
// 加速度
	//X,Y軸
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
	//Z軸
	for(i=4;i==4;i++){
			 if(analog_data[i]<   5) analog_data[i] = 0x00;
		else if(analog_data[i]<=  7) analog_data[i] = 0x01;
		else if(analog_data[i]<   9) analog_data[i] = 0x02;
		else if(analog_data[i]>=  9) analog_data[i] = 0x03;
	}
}
