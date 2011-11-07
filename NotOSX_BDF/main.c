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
	uint8_t _group,_channel; 

	union controller_data *controller;

	int8_t slaveBuf[16]={0},m_size;

	_group=0;
	_channel=0;

	m_size = 4;
	int8_t L_duty=0,Z_duty=0,Z_action,L_action;
	volatile int x,y;
	int8_t l_tire_table[15][15] =  {
	{   0,   4,  12,  24,  41,  61,  86, 100, 100, 100, 100, 100, 100, 100, 100},
	{   0,   0,	  4,  12,  24,	41,	 61,  86, 100, 100, 100, 100, 100, 100, 100},
	{   0,   0,   0,   4,  12,	24,	 41,  61,  86, 100, 100, 100, 100, 100, 100},
	{   0,   0,	  0,   0,	4,	12,	 24,  41,  61,	86, 100, 100, 100, 100, 100},
	{   0,   0,	  0,   0,	0,	 4,  12,  24,  41,	61,	 86, 100, 100, 100, 100},
	{   0,	 0,   0,   0,	0,	 0,	  4,  12,  24,	41,	 61,  86, 100, 100, 100},
	{   0,   0,	  0,   0,	0,	 0,	  0,   4,  12,	24,	 41,  61,  86, 100, 100},
	{   0, 	 0,	  0,   0,	0,	 0,	  0,   0,	4,	12,	 24,  41,  61,	86, 100},
	{   0,   0,	  0,   0,	0,	 0,	 -4,  -4,   0,	 8,	 20,  37,  57,	82, 100},
	{   0,   0,	  0,   0,	0,	-8,	-12, -12,  -8,	 0,	 12,  29,  49,	73,	100},
	{   0, 	 0,	  0,   0, -12, -20,	-24, -24, -20, -12,	  0,  16,  37,	61,	 90},
	{	0,	 0,	  0, -16, -29, -37,	-41, -41, -37, -29,	-16,   0,  20,	45,	 73},
	{	0,	 0,	-20, -37, -49, -57,	-61, -61, -57, -49,	-37, -20,   0,  24,	 53},
	{	0, -24,	-45, -61, -73, -82,	-86, -86, -82, -73,	-61, -45, -24,	 0,	 29},
	{ -29, -53,	-73, -90,-100,-100,-100,-100,-100,-100,	-90, -73, -53, -29,	  0}};

	int8_t z_tire_table[15][15] = {
	{ 100, 100,	100, 100, 100, 100,	100, 100,  86,	61,	 41,  24,  12,	 4,	  0},
	{ 100, 100,	100, 100, 100, 100,	100,  86,  61,	41,	 24,  12,	4,	 0,	  0},
	{ 100, 100,	100, 100, 100, 100,	 86,  61,  41,	24,	 12,   4,	0,	 0,	  0},
	{ 100, 100,	100, 100, 100,	86,	 61,  41,  24,	12,	  4,   0,	0,	 0,	  0},
	{ 100, 100,	100, 100,  86,	61,	 41,  24,  12,   4,	  0,   0,	0,	 0,	  0},
	{ 100, 100,	100,  86,  61,	41,	 24,  12,	4,   0,	  0,   0,	0,	 0,	  0},
	{ 100, 100,  86,  61,  41,	24,	 12,   4,	0,   0,	  0,   0,	0,	 0,	  0},
	{ 100,	86,	 61,  41,  24,	12,	  4,   0,	0,   0,	  0,   0,	0,	 0,	  0},
	{ 100,	82,	 57,  37,  20,	 8,	  0,  -4,  -4,   0,	  0,   0,	0,	 0,	  0},
	{ 100,	73,	 49,  29,  12,	 0,	 -8, -12, -12,  -8,	  0,   0,	0,	 0,	  0},
	{  90,	61,	 37,  16,	0, -12,	-20, -24, -24, -20,	-12,   0,	0,	 0,	  0},
	{  73,	45,	 20,   0, -16, -29,	-37, -41, -41, -37,	-29, -16,	0,	 0,	  0},
	{  53,	24,	  0, -20, -37, -49,	-57, -61, -61, -57,	-49, -37, -20,	 0,	  0},
	{  29,	 0,	-24, -45, -61, -73,	-82, -86, -86, -82,	-73, -61, -45, -24,	  0},
	{	0, -29,	-53, -73, -90,-100,-100,-100,-100,-100,-100, -90, -73, -53,	-29}};

	Slave Motor = {MOTOR,{(int8_t*)&slaveBuf[0],m_size},{(int8_t*)&slaveBuf[m_size],m_size}};
	Slave Throw = {THROW,{(int8_t*)&slaveBuf[10],1},{(int8_t*)&slaveBuf[11],1}};
#if SUPPLY_WATCHING
	Slave EStop = {ESTOP,{(int8_t*)&slaveBuf[12],1},{(int8_t*)&slaveBuf[13],1}};		
	uint8_t e_flag=0;
#endif
	
	bool act=false,i2cStatus;
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
	mu2_command("EI","08");
	mu2_command("DI","80");
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
				LED(2,mu2_command_eeprom("GI",group[_group]));
				LED(2,mu2_command_eeprom("CH",channel[_channel]));
				LED(2,mu2_command_eeprom("EI","08"));
				LED(2,mu2_command_eeprom("DI","80"));
				LED(2,mu2_command_eeprom("BR","48"));
				wait_ms(100);
				uart_init(0,UART_RE|UART_RXCIE,BR_4800);
			}
/*-------------------------------------------------------------------------*/


		L_duty = 0;
		Z_duty = 0;
		L_action=CW;
		Z_action=CW;
		if(controller->detail.Button.LEFT){
			L_duty = 50;Z_duty = 54;
			}
		else if(controller->detail.Button.RIGHT){
			L_duty =-53;Z_duty =-50;
			}
		else if(controller->detail.Button.L){
			L_duty= 55;Z_duty= -50;
/*さいくろん*/
			if(controller->detail.Button.ZL){
				L_duty=100;
				Z_duty= 0;
				}
			if(controller->detail.Button.X){
					L_duty=100;
					Z_duty= -100;
				}
			}
			
		else if(controller->detail.Button.R){
			L_duty= -50;Z_duty= 50;
/*さいくろん*/
			if(controller->detail.Button.ZR){
				Z_duty=100;
				L_duty=0;
				}
			if(controller->detail.Button.X){
				L_duty=-100;
				Z_duty= 100;
				}
			}
		else {
			x = controller->detail.AnalogL.X;
			y = controller->detail.AnalogL.Y;
/*if you want to use analog stick*/
/*			L_duty=l_tire_table[y][x];
			Z_duty=z_tire_table[y][x];
			Z_action=CW;
			L_action=CW;
			L_duty /= 2;
			Z_duty /= 2;
*/
			if(x==7&&y==7) { Z_action=BRAKE; L_action=BRAKE; }
		}
		if(Z_duty==0) { Z_action=BRAKE; }
		if(L_duty==0) { L_action=BRAKE; }
		mDrive(&Motor,Z_action,Z_duty,0);
		mDrive(&Motor,L_action,-L_duty,1);
		mDrive(&Motor,Z_action,Z_duty,2);
		mDrive(&Motor,L_action,-L_duty,3);
/*AIR*/
		if(controller->detail.Button.Y) act=1;
		else act=0;
		aDrive(&Throw,0x03,act);

#if SUPPLY_WATCHING
/*バルスモード*/
		/*バルス!!!!!*/
		if(controller->detail.Button.SELECT&&controller->detail.Button.START) e_flag=E_ON;
		/*復活!!!!!!!*/
		else if(controller->detail.Button.HOME) e_flag=E_OFF;
		else e_flag=E_KEEP;	
#endif

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
