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
	uint8_t _group=0,_channel=0; 

	union controller_data *controller;

	int8_t slaveBuf[16]={0},m_size=2;
	int8_t velocity;
	bool lAirAction, rAirAction;
	bool zlPushed, zrPushed, rightPushed, leftPushed;
	uint8_t lAirTimeCount, rAirTimeCount;
	bool i2cStatus;

	uint8_t port;
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
		// velocity = 0
		velocity = 0;
		// もし，aボタンが押下なら
			// velocity = 100;
		if( controller->detail.Button.A )
			{
				velocity = 100;
			}
		// そうでなく，もし，xボタンが押下なら
			// velocity = 70;
		else if( controller->detail.Button.X )
			{
				velocity = 70;
			}
		// そうでなく，もし，yボタンが押下なら
			// velocity = 30;
		else if( controller->detail.Button.Y )
			{
				velocity = 30;
			}
		// そうでなく，もし，bボタンが押下なら
			// velocity = -100;
		else if( controller->detail.Button.B )
			{
				velocity = -100;
			}
		// そうでなければ
			// velocity = 0;
		else
			{
				velocity = 0;
			}
		// MotorDrive( , CW, velocity, 0);
		mDrive(&Motor, CW, velocity, 0);
		// MotorDrive( , CW, velocity, 1);
		mDrive(&Motor, CW, velocity, 1);

		// lAirAction = rAirAction = OFF
		lAirAction = rAirAction = OFF;
		// zlPushed = zrPushed = rightPushed = leftPushed = false
		zlPushed = zrPushed = rightPushed = leftPushed = false;
		// lAirTimeCount = rAirTimeCount = 0
		lAirTimeCount = rAirTimeCount = 0;

		// もし，Lボタンが押下なら
			// lAirAction = ON
		if( controller->detail.Button.L )
			{
				lAirAction = ON;
			}
		// そうでなく，ZLボタンが押下．かつzlPushedga偽なら
			// lAirAction = ON
			// zlPushed = 真
			// lAirTimeCount = □
		else if( controller->detail.Button.ZL && ! zlPushed )
			{
				lAirAction = ON;
				zlPushed = true;
				lAirTimeCount = 10;
			}
		// そうでなく，ZLボタンが押下でなく．かつzlPushedが真なら
			// zlPushed = 偽
		else if( ! controller->detail.Button.ZL && zlPushed )
			{
				zlPushed = false;
			}
		// そうでなく，←ボタンが押下で，かつleftPushedが偽なら
			// lAirAction = ON
			// leftPushed = 真
			// lAirTimeCount = ○
		else if( controller->detail.Button.LEFT && ! leftPushed )
			{
				lAirAction = ON;
				leftPushed = true;
				lAirTimeCount = 20;
			}
		// そうでなく，←ボタンが押下でなく．かつleftPushedが真なら
			// leftPushed = 偽
		else if( ! controller->detail.Button.LEFT && leftPushed )
			{
				leftPushed = false;
			}
		// そうでなく，lAirTimeCountが0なら
			// lAirAction = OFF
		else if( lAirTimeCount == 0 )
			{
				lAirAction = OFF;
			}
		// もし，lAirTimeCountが0でなければ
			// lAirTimeCountをデクリメント
		if( lAirTimeCount )
			{
				lAirTimeCount--;
			}
		// AirDrive()

		// もしRボタンが押下なら
			// rAirAction = ON
		if( controller->detail.Button.R )
			{
				rAirAction = ON;
			}
		// そうでなく，ZRボタンが押下．かつzrPushedが偽なら
			// rAirAction = ON
			// zrPushed = 真
			// rAirTimeCount = □
		else if( controller->detail.Button.ZR && ! zrPushed )
			{
				rAirAction = ON;
				zrPushed = true;
				rAirTimeCount = 10;
			}
		// そうでなく，ZRボタンが押下でなく，かつzrPushedが真なら
			// zrPushed = 偽
		else if( ! controller->detail.Button.ZR && zrPushed )
			{
				zrPushed = false;
			}
		// そうでなく，→ボタンが押下で，かつrightPushedが偽なら
			// rAirAction = ON
			// rightPushed = 真
			// rAirTimeCount = ○
		else if( controller->detail.Button.RIGHT && ! rightPushed )
			{
				rAirAction = ON;
				rightPushed = true;
				rAirTimeCount = 20;
			}
		// そうでなく，→ボタンが押下でなく．かつrightPushedが真なら
			// rightPushed = 偽
		else if( ! controller->detail.Button.RIGHT && rightPushed )
			{
				rightPushed = false;
			}
		// そうでなく，rAirTimeCountが0なら
			// rAirTimeCountをデクリメント
		else if( rAirTimeCount == 0 )
			{
				rAirTimeCount--;
			}
		// AirDrive()
		port = ( ( rAirAction << 3 ) | ( lAirAction << 2 ) | ( rAirAction << 1 ) | lAirAction );
		aDrive(&Throw,port, ( rAirAction | lAirAction ) );

//
///*AIR*/
//		if(a_flag){
//	/*AIR ON*/
//			port=0x00;
//			if(controller->detail.Button.L) port|=0x01;
//			else port&=~0x01;
//			if(controller->detail.Button.R) port|=0x02;
//			else port&=~0x02;
//	/*一瞬エアON(6ms)*/
//			if(controller->detail.Button.ZL) port|=0x10;
//			else port&=~0x10;
//			if(controller->detail.Button.ZR) port|=0x20;
//			else port&=~0x20;
//			}
//	/*最初に片方だけあげたままにする&戻す*/
//		if(controller->detail.Button.UP)  {port=0x02;a_flag=0;}
//		if(controller->detail.Button.DOWN){port=0x00;a_flag=1;}
//		if(port) act=1;
//		else act=0;
//		LED(2,act);
//		aDrive(&Throw,port,act);
		
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
		i2cStatus = true;
#endif

/*I2C Writeing And Check*/

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
