#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <i2c.h>
#include <uart.h>
#include <led.h>
#include <switch.h>
#include <mu2.h>
#include <remote_control_rx.h>
#include <wait.h>
#include <actuater.h>
#include <beep.h>
#include <emergency.h>


#define SUPPLY_WATCHING (1)

void initSlaveSupply(void)
{
	DDRC |= 0x04;
	PORTC = 0x04;
}
void supplySalve(void)
{
	
	PORTC &= 0xFB;
} 

int main(void)
{

	union controller_data *controller;

/*---------------------------------------------------------------*/
// 						èëÇ´Ç©Ç¶ÇøÇ·É_ÉÅÇÊÅI
/*---------------------------------------------------------------*/
	LEDInit();
	SwitchInit();
	BeepInit();
	initCtrlData();
	initSlaveSupply();
	initI2CMaster(100);

	setMU2PutFunc(uart0Put);
	setMU2GetFunc(uart0Get);

	wdt_reset();
	wdt_enable(WDTO_500MS);
	
	wait_ms(100);
	initUART(0,
			 StopBitIs1Bit|NonParity,
			 ReceiveEnable|TransmitEnable,
			 19200);
	mu2Command("GI","01");
	mu2Command("CH","08");
	mu2Command("EI","01");
	mu2Command("DI","10");
	mu2Command("BR","48");
	initUART(0,
			 StopBitIs1Bit|NonParity,
			 ReceiveEnable|TransmitEnable|ReceiveCompleteInteruptEnable,
			 4800);
	LED(0,false);LED(1,false);LED(2,false);
	sei();
	wait_ms(1500);
	supplySalve();
	wait_ms(25);


	while(1){	
		controller = Toggle_RC_Rx_Buffer();
		
		wait_ms(15);
	}
	return 0;
}
