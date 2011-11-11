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

//SLAVE ADDR
#define MOTOR 0x01
#define ESTOP 0x10 
#define AIR	  0x40
#define THROW 0x40
#define CATCH 0x50

int main(void)
{
	union controller_data *controller;
	int8_t slaveBuf[12]={0};
	bool carveEnable;
	int16_t carveEnableCount;
	int16_t timeCount;
	int16_t motorDuty;
	int16_t blueDuty;
	int16_t orangeDuty;
	uint8_t blueAct;
	uint8_t orangeAct;
	uint8_t airAct;
	uint8_t airPort;
	uint8_t e_flag;

/*
	unsigned char action, blueAction, orangeAction, e_flag, AirPort;
	int16_t blueDuty;
	int16_t orangeDuty;
	int16_t count = 0;
*/

	Slave Motor = {MOTOR,{(int8_t*)&slaveBuf[0],4},{(int8_t*)&slaveBuf[4],4}};
	Slave Air	= {AIR,{(int8_t*)&slaveBuf[8],1},{(int8_t*)&slaveBuf[9],1}};
	Slave Emer	= {ESTOP,{(int8_t*)&slaveBuf[10],1},{(int8_t*)&slaveBuf[11],1}};

	/*この辺にi2c関連の初期化をすればいいと思う*/

	bool i2cStatus;
	initCtrlData();

	TCCR2A = 0;
	TCCR2B = 1;
/*---------------------------------------------------------------*/
// 						書きかえちゃダメよ！
/*---------------------------------------------------------------*/
	LEDInit();
	SwitchInit();
	BeepInit();
	DDRC |= 0x04;
	PORTC = 0x04;
	
		
	initI2CMaster(100);

	wdt_reset();
	wdt_enable(WDTO_500MS);
	
	uart_init(1,UART_TE,BR_115200);
	uart_setbuffer(0,buf,36);
	wait_ms(100);
	uart_init(0,UART_RE|UART_TE,BR_19200);
	mu2_command("EI","08");
	mu2_command("DI","80");
	mu2_command("GI","04");
	mu2_command("CH","2E");
	mu2_command("BR","48");
	uart_init(0,UART_RE|UART_RXCIE,BR_4800);
	LED(0,false);LED(1,false);LED(2,false);
	sei();
	wait_ms(1500);
	PORTC &= 0xFB;
//	wait_ms(25);
	TCCR1A = 0;
	TCCR1B = 5;
	TIMSK1 = 1;
	TCNT1 = 0;

// Entry
// 	carveEnable = false
	carveEnable = false;
// 	carveEnableCount = 0
	carveEnableCount = 0;
//	timeCount = 0
	timeCount = 0;

	while(1){	
		controller = Toggle_RC_Rx_Buffer();
		if(controller->detail.Button.HOME&&controller->detail.Button.X && controller->detail.Button.UP&&
			controller->detail.AnalogL.Y >12 &&	controller->detail.AnalogR.Y >12){
				LED(0,false);LED(1,false);LED(2,false);
				uart_init(0,UART_RE|UART_TE,BR_4800);			
				LED(2,mu2_command_eeprom("EI","08"));		
				LED(2,mu2_command_eeprom("DI","80"));		
				LED(2,mu2_command_eeprom("GI","04"));
				LED(2,mu2_command_eeprom("CH","2E"));
				LED(2,mu2_command_eeprom("BR","48"));
				wait_ms(100);
				uart_init(0,UART_RE|UART_RXCIE,BR_4800);
			}
/*-------------------------------------------------------------------------*/
		/*ここにプログラムを記述するとよろしいのではないのかと思われます*/
/* 新式 */

// Do
// もし上ボタンが押されていないなら
	if( ! controller->detail.Button.UP )
		{
			// もしcarveEnableCountが○未満ならcarveEnableCountをインクリメント
			// そうでなければcarveEnableを真にする
				if( carveEnableCount < 40 )
					{			
						carveEnableCount++;
					}
				else
					{
						carveEnable = true;
					}
			// motorDutyをBボタンが押されていたら30,そうでなければ100
				motorDuty = ( controller->detail.Button.B ) ? 30:100;
			
			// もしLボタンが押されていたら、全駆動体に+motorDutyをセット
				if( controller->detail.Button.L )
					{
						blueDuty = +motorDuty;
						orangeDuty = +motorDuty;
					}
			// そうでなくRボタンが押されていたら、全駆動体に-motorDutyをセット
				else if( controller->detail.Button.R )
					{
						blueDuty = -motorDuty;
						orangeDuty = -motorDuty;
					}
			// そうでなく←ボタンが押されていたら、青の駆動体は正、橙の駆動体は負にmotorDutyをセット
				else if( controller->detail.Button.LEFT )
					{
						blueDuty = +motorDuty;
						orangeDuty = -motorDuty;
					}
			// そうでなく→ボタンが押されていたら、青の駆動体は負、橙の駆動体は正にmotorDutyをセット
				else if( controller->detail.Button.RIGHT )
					{
						blueDuty = -motorDuty;
						orangeDuty = +motorDuty;
					}
			// そうでなければ、停止
				else
					{
						blueDuty = 0;
						orangeDuty = 0;
					}
		}

// そうでなければ (↑ボタンが押下） かつ carveEnable が真なら
	else if( carveEnable )
		{
			// もし timeCount が□未満なら
				if( timeCount < 105 )
					{
						// もしtimeCount が△未満なら、全駆動体を-motorDutyにセット
						// そうでなければ、徐々に曲げる
							if( timeCount < 20 )
								{
									blueDuty = -100;
									orangeDuty = -100;
								}
							else
								{
									blueDuty = -100;
									orangeDuty = 70+timeCount;
									if( orangeDuty < -100 ) orangeDuty = -100;
									else if( orangeDuty > 100 ) orangeDuty = 100;
								}
						// timeCountをインクリメント
							timeCount++;
					}
			// そうでなければ(timeCount が□以上)、停止
				else
					{
						blueDuty = 0;
						orangeDuty = 0;
					}
		}
// そうでなければ (↑ボタンが押下 かつ carveEnable が偽)、停止
	else 
		{
			blueDuty = 0;
			orangeDuty = 0;
		}

// もし blueDuty が0なら、青の駆動体をブレーキ、そうでなければCW
	blueAct = ( blueDuty == 0 || blueDuty == -0 ) ? BRAKE : CW;

// もし orangeDuty が0なら、橙の駆動体をブレーキ、そうでなければCCW
	orangeAct = ( orangeDuty == 0 || orangeDuty == -0 ) ? BRAKE : CCW;

//	モータドライブ関数をコール
	mDrive(&Motor, blueAct, blueDuty, 0);
	mDrive(&Motor, blueAct, blueDuty, 1);
	mDrive(&Motor, orangeAct, orangeDuty, 2);
	mDrive(&Motor, orangeAct, orangeDuty, 3);

//	i2cで転送

 // 旧式
/*
		if(controller->detail.Button.UP&&count<113){
			uDuty = 100;
			dDuty = 100;
			count++;
			if(count>27) dDuty =  -65-count;
			if(dDuty<-100) dDuty = -100;
			mDrive(&Motor,CCW,uDuty,0);	//BLUE
			Motor.write.buf[1] = Motor.write.buf[0];
			mDrive(&Motor,CW,(int8_t)dDuty,2);	//ORANGE
			mDrive(&Motor,CW,(int8_t)dDuty,3);	//ORANGE
		}else{
			if(	controller->detail.AnalogR.Y<0x05	||
				controller->detail.AnalogL.Y<0x05	)
				{
					uDuty = 0;
					dDuty = 100;
				}
			else if(	controller->detail.AnalogR.Y>0x09	||
						controller->detail.AnalogL.Y>0x09	)
				{
					uDuty = 100;
					dDuty = 0;
				}
			else if(controller->detail.Button.B)
				{
					uDuty = dDuty = 30;
				}
			else
				{
					uDuty = dDuty = 100;
				}
		
			if(	controller->detail.Button.L			||
				controller->detail.AnalogR.X<0x05	||
				controller->detail.AnalogL.X<0x05	)
				{
					uAction=CW;
					dAction=CCW;
				}
			else if( 	controller->detail.Button.R			||
						controller->detail.AnalogR.X>0x09	||
						controller->detail.AnalogL.X>0x09	)
				{
					uAction=CCW;
					dAction=CW;
				}
			else if(controller->detail.Button.LEFT)
				{
					uAction = dAction = CW;
				}
			else if(controller->detail.Button.RIGHT)
				{
					uAction = dAction = CCW;
				}
			else
				{
					uAction = dAction = BRAKE;
				}

			mDrive(&Motor,uAction,uDuty,0);	//BLUE
			Motor.write.buf[1] = Motor.write.buf[0];
			mDrive(&Motor,dAction,(int8_t)dDuty,2);	//ORANGE
			mDrive(&Motor,dAction,(int8_t)dDuty,3);	//ORANGE
		}
*/
/*-------------------------------------------------------------------------------------------------------------------------*/
		airAct = FREE;
		if(controller->detail.Button.Y)
			{
				airAct = CW;
				airPort = 0x03;
			}
		else if(controller->detail.Button.ZL)
			{
				airAct = CW;
				airPort = 0x01;
			}
		else if(controller->detail.Button.ZR)
			{
				airAct = CW;
				airPort = 0x02;
			}
		aDrive(&Air, airPort, airAct);
/*-------------------------------------------------------------------------------------------------------------------------*/
		/*バルスモード*/
		/*バルス!!!!!*/
		if(controller->detail.Button.SELECT&&controller->detail.Button.START) e_flag=E_ON;
		/*復活!!!!!!!*/
		else if(controller->detail.Button.HOME){
			e_flag=E_OFF;
			carveEnable = false;
			carveEnableCount = 0;
			timeCount = 0;
		}
		else e_flag=E_KEEP;	

		i2cStatus = true;
		i2cStatus &= Emergency(&Emer,e_flag);
		i2cStatus &= i2cWrite(&Motor); wait_us(10);
		i2cStatus &= i2cWrite(&Air); wait_us(10);
		i2cStatus &= i2cWrite(&Emer);

		i2cCheck(i2cStatus);
		wait_ms(25);
	}
	return 0;
}
