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
	unsigned char TxDataBuf[RC_DATA_LENGTH*5];				//uartの送信用バッファここに一時的に送信するデータを蓄える
	unsigned char Group[4] 		 = {0x21,0x9A,0x44,0x80};	//グループリスト
	unsigned char Channel[4][4]  ={{0x07,0x08,0x09,0x0A},	//チャンネルリスト
							 	   {0x0C,0x0D,0x0E,0x0F},
								   {0x11,0x12,0x13,0x14},
								   {0x16,0x17,0x18,0x19}};
								   	
	union wii_nunchuk data;									//wiiコントローラーから受けとるデータ
	union controller_data rcdata;							//MU2に送るデータ

    uart_init( UART_RE|UART_TE, 19200);						//受信許可|送信許可|送信データ空割り込み許可，ボーレート
    uart_setbuffer(TxDataBuf,RC_DATA_LENGTH*5);				//送信用バッファの設定
    
    MU2_SetRxHandler(uart_getchar);							//uartの1byte受信関数をセット
    MU2_SetTxHandler(uart_putchar);							//uartの1byte送信予約関数をセット引数にuart_putcharを 
															//入れると割り込みではなくwait形式になる			  	
	MU2_SetGroupID(Group[SWITCH>>2]);						//グループ設定
	MU2_SetChannel(Channel[SWITCH>>2][SWITCH & 0x03]);		//チャンネル設定

	while(1){
		set_controller();									//コントローラー初期化				
		get_value( data.buf );								//コントローラーから値取得(I2C)
		get_analog(&data,analog_data);						//アナログスティックと加速度センサの値を取得
		MakeRCData(&rcdata,&data,analog_data);							//rcdataのバッファに値を入れる
		MU2_SendData(rcdata.buf,RC_DATA_LENGTH);			//MU2でデータ送信
		wait(45);											//wait(45ms)
	}
    return 0;
}
