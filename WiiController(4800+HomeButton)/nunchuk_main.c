#include <avr/io.h>
#include "uart/uart.h"
#include "mu2/mu2.h"
#include "nunchuk/nunchuk.h"
#include "nunchuk/rc_nunchuk.h"
#include <avr/wdt.h>

void set_controller();
void get_value(unsigned char *);
void get_nunchuk_analog(const union wii_nunchuk *, unsigned char *);
void MakeNunchukRCData(union nunchuk_data *rcdata,const union wii_nunchuk *data, unsigned char *);
void wait(uint16_t);
void battery_check();

int nunchuk(int mode,unsigned char *analog_data)
{
	union wii_nunchuk data;										
	union nunchuk_data rcdata;										
	while(1){
		wdt_reset();
		set_controller();									//コントローラー初期化				
		get_value( data.buf );								//コントローラーから値取得(I2C)
		get_nunchuk_analog(&data,analog_data);				//アナログスティックと加速度センサの値を取得
		MakeNunchukRCData(&rcdata,&data,analog_data);		//rcdataのバッファに値を入れる
		if(mode==0)	MU2_SendData(rcdata.buf,NUNCHUK_DATA_LENGTH);//MU2でデータ送信
		if(mode==1)	MU2_SendDataBus(rcdata.buf,NUNCHUK_DATA_LENGTH);//Send Data for Serial
		wait(45);
		if(rcdata.detail.Button.C&&rcdata.detail.Button.Z) battery_check();
	}
    return 0;
}
