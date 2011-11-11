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

	/*���̕ӂ�i2c�֘A�̏�����������΂����Ǝv��*/

	bool i2cStatus;
	initCtrlData();

	TCCR2A = 0;
	TCCR2B = 1;
/*---------------------------------------------------------------*/
// 						������������_����I
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
		/*�����Ƀv���O�������L�q����Ƃ�낵���̂ł͂Ȃ��̂��Ǝv���܂�*/
/* �V�� */

// Do
// ������{�^����������Ă��Ȃ��Ȃ�
	if( ! controller->detail.Button.UP )
		{
			// ����carveEnableCount���������Ȃ�carveEnableCount���C���N�������g
			// �����łȂ����carveEnable��^�ɂ���
				if( carveEnableCount < 40 )
					{			
						carveEnableCount++;
					}
				else
					{
						carveEnable = true;
					}
			// motorDuty��B�{�^����������Ă�����30,�����łȂ����100
				motorDuty = ( controller->detail.Button.B ) ? 30:100;
			
			// ����L�{�^����������Ă�����A�S�쓮�̂�+motorDuty���Z�b�g
				if( controller->detail.Button.L )
					{
						blueDuty = +motorDuty;
						orangeDuty = +motorDuty;
					}
			// �����łȂ�R�{�^����������Ă�����A�S�쓮�̂�-motorDuty���Z�b�g
				else if( controller->detail.Button.R )
					{
						blueDuty = -motorDuty;
						orangeDuty = -motorDuty;
					}
			// �����łȂ����{�^����������Ă�����A�̋쓮�̂͐��A��̋쓮�͕̂���motorDuty���Z�b�g
				else if( controller->detail.Button.LEFT )
					{
						blueDuty = +motorDuty;
						orangeDuty = -motorDuty;
					}
			// �����łȂ����{�^����������Ă�����A�̋쓮�͕̂��A��̋쓮�̂͐���motorDuty���Z�b�g
				else if( controller->detail.Button.RIGHT )
					{
						blueDuty = -motorDuty;
						orangeDuty = +motorDuty;
					}
			// �����łȂ���΁A��~
				else
					{
						blueDuty = 0;
						orangeDuty = 0;
					}
		}

// �����łȂ���� (���{�^���������j ���� carveEnable ���^�Ȃ�
	else if( carveEnable )
		{
			// ���� timeCount ���������Ȃ�
				if( timeCount < 105 )
					{
						// ����timeCount ���������Ȃ�A�S�쓮�̂�-motorDuty�ɃZ�b�g
						// �����łȂ���΁A���X�ɋȂ���
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
						// timeCount���C���N�������g
							timeCount++;
					}
			// �����łȂ����(timeCount �����ȏ�)�A��~
				else
					{
						blueDuty = 0;
						orangeDuty = 0;
					}
		}
// �����łȂ���� (���{�^�������� ���� carveEnable ���U)�A��~
	else 
		{
			blueDuty = 0;
			orangeDuty = 0;
		}

// ���� blueDuty ��0�Ȃ�A�̋쓮�̂��u���[�L�A�����łȂ����CW
	blueAct = ( blueDuty == 0 || blueDuty == -0 ) ? BRAKE : CW;

// ���� orangeDuty ��0�Ȃ�A��̋쓮�̂��u���[�L�A�����łȂ����CCW
	orangeAct = ( orangeDuty == 0 || orangeDuty == -0 ) ? BRAKE : CCW;

//	���[�^�h���C�u�֐����R�[��
	mDrive(&Motor, blueAct, blueDuty, 0);
	mDrive(&Motor, blueAct, blueDuty, 1);
	mDrive(&Motor, orangeAct, orangeDuty, 2);
	mDrive(&Motor, orangeAct, orangeDuty, 3);

//	i2c�œ]��

 // ����
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
		/*�o���X���[�h*/
		/*�o���X!!!!!*/
		if(controller->detail.Button.SELECT&&controller->detail.Button.START) e_flag=E_ON;
		/*����!!!!!!!*/
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
