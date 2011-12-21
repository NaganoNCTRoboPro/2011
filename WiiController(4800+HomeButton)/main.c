#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "uart/uart.h"
#include "mu2/mu2.h"
#include "nunchuk/rc_nunchuk.h"
#include "classic/rc_classic.h"

#define SWITCH	(~PINB & 0x0F)

void setup();
void set_adc();
void battery_states();
void wait(uint16_t);
int nunchuk(int,unsigned char *);
int classic(int,unsigned char *);
int mode();
int controller_select();

int main(void)
{
	int len,controller;

	setup();												//Port Setting
	set_adc();												//ADconvert Setting
	controller=controller_select();							//controller select(nunchuk or classic)										
	battery_states();

	if(controller==1)	len=NUNCHUK_DATA_LENGTH;			//data length  
	if(controller==0)	len=CLASSIC_DATA_LENGTH;			

	unsigned char TxDataBuf[len*5];							//uart send buffer
	unsigned char analog_data[5];	
	/*old version (2010)*/
/*	unsigned char Group[4] 		 = {0x21,0x9A,0x44,0x80};	//GroupID List
	unsigned char Channel[4][4]  ={{0x07,0x08,0x09,0x0A},	//Channel List
							 	   {0x0C,0x0D,0x0E,0x0F},
								   {0x25,0x26,0x27,0x28},
								   {0x2A,0x2B,0x2C,0x2D}};*/
	unsigned char Group[4] 	= {0x01,0x02,0x03,0x04};	//GroupID List
	unsigned char Channel[4] ={0x08,0x08,0x2E,0x2E};//Channel List
	unsigned char DI[4] = {0x01,0x02,0x04,0x08}; //ID
	unsigned char EI[4] = {0x10,0x20,0x40,0x80}; //ID

	uint8_t _group,_channel;

	uart_init(UART_RE|UART_TE,19200);						//受信許可|送信許可|送信データ空割り込み許可，ボーレート
	uart_setbuffer(TxDataBuf,len*5);						//送信用バッファの設定
  
    MU2_SetRxHandler(uart_getchar);							//uartの1byte受信関数をセット
    MU2_SetTxHandler(uart_putchar);							//uartの1byte送信予約関数をセット

//	MU2_SetGroupID(Group[SWITCH>>2]);						//Set GroupID
//	MU2_SetChannel(Channel[SWITCH>>2][SWITCH & 0x03]);		//Set Channel

				if(SWITCH&0x01){
					_group=0;
					_channel=0;
					}
				else if(SWITCH&0x02){
					_group=1;
					_channel=1;
					}
				else if(SWITCH&0x04){
					_group=2;
					_channel=2;
					}
				else{
					_group=3;
					_channel=3;
					}	
	MU2_SetGroupID(Group[_group]);
	MU2_SetChannel(Channel[_channel]);
	MU2_SetEquipmentID(EI[_group]);
	MU2_SetDestinationID(DI[_group]);

	wait(500);
	wait(50);
	uart_putchar('@');	
	uart_putchar('B');	
	uart_putchar('R');	
	uart_putchar('4');	
	uart_putchar('8');	
	uart_putchar(0x0d);	
	uart_putchar(0x0a);
	wait(500);
	uart_init( UART_RE|UART_TE,4800);						//受信許可|送信許可|送信データ空割り込み許可，ボーレート
	wait(500);
	wdt_reset();
	wdt_disable();

	if(controller==1){										//Nunchuk Mode
		PORTC &=~_BV(PC1);									//LED ON
		nunchuk(mode(),analog_data);						
	}
	if(controller==0){										//Classic Mode
		PORTC &=~_BV(PC2);									//LED ON
		classic(mode(),analog_data);
	}
	return 0;
}
