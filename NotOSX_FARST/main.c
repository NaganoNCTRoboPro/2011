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

static uint8_t buf[36];


#define SUPPLY_WATCHING (1)

int main(void)
{

	union controller_data *controller;

/*---------------------------------------------------------------*/
// 						‘‚«‚©‚¦‚¿‚áƒ_ƒ‚æI
/*---------------------------------------------------------------*/
	LEDInit();
	SwitchInit();
	BeepInit();
	initCtrlData();
	DDRC |= 0x04;
	PORTC = 0x04;
	initI2CMaster(100);

	wdt_reset();
	wdt_enable(WDTO_500MS);
	
	uart_init(1,UART_TE,BR_115200);
	uart_setbuffer(0,buf,36);
	wait_ms(100);
	uart_init(0,UART_RE|UART_TE,BR_19200);
	mu2_command("GI","01");
	mu2_command("CH","08");
	mu2_command("EI","01");
	mu2_command("DI","10");
	mu2_command("BR","19");
	uart_init(0,UART_RE|UART_RXCIE,BR_19200);
	LED(0,false);LED(1,false);LED(2,false);
	sei();
	wait_ms(100);
	PORTC &= 0xFB;
	wait_ms(25);

	TCCR1A = 0;
	TCCR1B = 5;
	TIMSK1 = 1;
	TCNT1 = 0;

	while(1){	
		controller = Toggle_RC_Rx_Buffer();
		
		LED(2,controller->detail.Button.A);
/********‚±‚êˆÈ~‚ğ‘‚«Š·‚¦‚é‚±‚Æ‚Í„§‚³‚ê‚È‚¢‚æ!!!********/
		wait_ms(25);
	}
	return 0;
}
