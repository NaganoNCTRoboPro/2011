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

void wait(uint16_t w)	//約１ms単位
{
	while(w--){
		volatile uint16_t i=160;
		while(i--);
	}
}

void error()				//エラー処理
{
	while(1){
		PORTD |= _BV(PD7);	//電源LEDを点滅させる
		wait(200);
		PORTD &=~_BV(PD7);
		wait(200);
		PORTD |= _BV(PD7);
	}
}

void setup()									//マイコンの初期化
{
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

void get_analog( const union wii_classic *data, unsigned char *analog_data )
{
	int i;	
	
	//各アナログスティックの値をanalog_data[]に格納
	analog_data[0] = (unsigned char)GetAnalogueLeftX( data );
	analog_data[1] = (unsigned char)GetAnalogueLeftY( data );
	analog_data[2] = (unsigned char)GetAnalogueRightX( data );
	analog_data[3] = (unsigned char)GetAnalogueRightY( data );

	//アナログスティックの値の処理(左)
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
	
	//アナログスティックの値の処理(右)
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

