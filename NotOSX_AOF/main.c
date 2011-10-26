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

static char *group[4]   = {"01","02","03","04"};
static char *channel[4] = {"08","20","22","2E"};
static uint8_t buf[36];

//SLAVE ADDR
#define MOTOR 0x01
#define ESTOP 0x10 
#define THROW 0x40
#define CATCH 0x50

#define SUPPLY_WATCHING (1)

int main(void)
{
	union controller_data *controller;

	uint8_t port,_group=0,_channel=0,action,i; 
	int8_t slaveBuf[16]={0},m_size=2,duty=0;
	bool a_flag=true,act=false,i2cStatus;

	Slave Motor = {MOTOR,{(int8_t*)&slaveBuf[0],m_size},{(int8_t*)&slaveBuf[m_size],m_size}};
	Slave Throw = {THROW,{(int8_t*)&slaveBuf[10],1},{(int8_t*)&slaveBuf[11],1}};
#if SUPPLY_WATCHING
	Slave EStop = {ESTOP,{(int8_t*)&slaveBuf[12],1},{(int8_t*)&slaveBuf[13],1}};		
	uint8_t e_flag=0;
#endif

/*---------------------------------------------------------------*/
// 						書きかえちゃダメよ！
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
	mu2_command("GI",group[_group]);
	mu2_command("CH",channel[_channel]);
	mu2_command("EI","01");
	mu2_command("DI","10");
	mu2_command("BR","48");
	uart_init(0,UART_RE|UART_RXCIE,BR_4800);
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
		if(controller->detail.Button.HOME&&controller->detail.Button.X && controller->detail.Button.UP&&
			controller->detail.AnalogL.Y >12 &&	controller->detail.AnalogR.Y >12){
				LED(0,false);LED(1,false);LED(2,false);
				uart_init(0,UART_RE|UART_TE,BR_4800);
				LED(2,mu2_command_eeprom("EI","01"));
				LED(2,mu2_command_eeprom("DI","10"));
				LED(2,mu2_command_eeprom("GI",group[_group]));
				LED(2,mu2_command_eeprom("CH",channel[_channel]));
				LED(2,mu2_command_eeprom("BR","48"));
				wait_ms(100);
				uart_init(0,UART_RE|UART_RXCIE,BR_4800);
			}
/*-------------------------------------------------------------------------*/

/*AIR*/
		if(a_flag){
	/*AIR ON*/
			port=0x00;
			if(controller->detail.Button.L) port|=0x01;
			else port&=~0x01;
			if(controller->detail.Button.R) port|=0x02;
			else port&=~0x02;
	/*一瞬エアON(6ms)*/
			if(controller->detail.Button.ZL) port|= 0x10;
			else							 port&=~0x10;
			if(controller->detail.Button.ZR) port|= 0x20;
			else							 port&=~0x20;
			}
	/*最初に片方だけあげたままにする&戻す*/
		if(controller->detail.Button.UP)  {port=0x02;a_flag=0;}
		if(controller->detail.Button.DOWN){port=0x00;a_flag=1;}
		if(port) act=1;
		else act=0;
		LED(2,act);
		
#if SUPPLY_WATCHING
/*バルスモード*/
		/*バルス!!!!!*/
		if(controller->detail.Button.SELECT&&controller->detail.Button.START){
			e_flag=E_ON;
			port&=0xCF;
			}
		/*復活!!!!!!!*/
		else if(controller->detail.Button.HOME) e_flag=E_OFF;
		else e_flag=E_KEEP;	
#endif	
		aDrive(&Throw,port,act);
/*歩行*/		
		duty=100;
		if(controller->detail.Button.A) action=CW;
		else if(controller->detail.Button.B) action=CCW;
		else action=BRAKE;
	/*低速モード*/
		if(controller->detail.Button.X)		{duty=70;action=CW;}
		else if(controller->detail.Button.Y){duty=35;action=CW;}
	/*I2Cデータの生成*/
		for(i=0;i<2;i++) mDrive(&Motor,action,duty,i);			

/*I2C Writeing And Check*/
		i2cStatus = true;
		i2cStatus &= i2cWrite(&Motor); wait_us(4);  
#if SUPPLY_WATCHING
		i2cStatus &= Emergency(&EStop,e_flag);	
		i2cStatus &= i2cWrite(&EStop); wait_us(4);
#endif
		i2cStatus &= i2cWrite(&Throw); wait_us(4);
		i2cCheck(i2cStatus);

/********これ以降を書き換えることは推奨されないよ!!!********/
		wait_ms(25);
	}
	return 0;
}
