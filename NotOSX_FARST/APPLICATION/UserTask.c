#include <avr/interrupt.h>
#include <kernel.h>
#include <stdbool.h>
#include <led.h>
#include <i2c.h>
#include <wait.h>
#include <actuater.h>
#include "UserTask.h"

#include <avr/wdt.h>

#include "remote_control_rx.h"
#if CONTROLLER == SIXAXIS
	#include "rc_sixaxis.h"
#elif CONTROLLER == NUNCHUK
	#include "rc_nunchuk.h"
#elif CONTROLLER == CLASSIC
	#include "rc_classic.h"
#endif

static TCB userTask;
Stack userTaskStack[400 ];

static void UserTask(void);

void UserTaskInit(void)
{
	userTask.NextTask = NULL;
	userTask.Status = READY;
	userTask.Priority = 2;
	userTask.pTask = &UserTask;
	userTask.pStack = userTaskStack + DEFAULT_STACK_SIZE;
	userTask.StackSize = DEFAULT_STACK_SIZE;
	userTask.ID = 2;

	AddTaskList(&userTask);
}

volatile static int8_t dutyLine[] = {
	100,95,85,70,50,25,10,0,-10,-25,-50,-70,-85,-95,-100
};

volatile int8_t slaveBuf[10] = {0,0,0,0,0,0,0,0,0,0};
Slave FTire = {
	0x01,
	{(uint8_t*)&slaveBuf[0],1},
	{(uint8_t*)&slaveBuf[1],1}
};
Slave BTire = {
	0x02,
	{(uint8_t*)&slaveBuf[2],1},
	{(uint8_t*)&slaveBuf[3],1}
};
Slave AirCylinder = {
	0x20,
	{(uint8_t*)&slaveBuf[4],1},
	{(uint8_t*)&slaveBuf[5],1}
};
Slave FServo = {
	0x40,
	{(uint8_t*)&slaveBuf[6],1},
	{(uint8_t*)&slaveBuf[7],1}
};
Slave BServo = {
	0x40,
	{(uint8_t*)&slaveBuf[8],1},
	{(uint8_t*)&slaveBuf[9],1}
};

static void UserTask(void)
{
	union controller_data *controller;
	bool connection = false;
	int8_t duty = 0;
	int16_t angle = 90;
	sei();
	*FTire.write.buf = 0;
	*BTire.write.buf = 0;
	wdt_reset();
	i2cWrite(&FTire);wait_us(4);
//	i2cWrite(&BTire);wait_us(4);
	i2cWrite(&FTire);wait_us(4);
//	i2cWrite(&BTire);
	Sleep(25);
	wdt_reset();
	sei();
	while(1)
	{	
		controller = Toggle_RC_Rx_Buffer();

		if(	controller->detail.Button.START	||
			controller->detail.Button.A		||
			controller->detail.Button.B			) { LED(2,true);}
		else LED(2,false);

		angle = 90+((7-controller->detail.AnalogL.X)*13);
		if(angle>180) angle = 180;
		else if(angle<0) angle = 0;
		
		if(controller->detail.Button.A){
				duty = 100;
			}
		else if(controller->detail.Button.B){
				duty = -100;
			}
		else {
				duty = 0;
			}

		MotorDrive(&FTire,CW,duty);
		MotorDrive(&BTire,CW,duty);
		ServoDrive(&FServo,angle,0);
		ServoDrive(&BServo,180-angle,1);
		AirCylinderDrive(&AirCylinder,0,controller->detail.Button.Y);
		AirCylinderDrive(&AirCylinder,1,controller->detail.Button.Y);

		connection = true;

		if(i2cWrite(&FTire)) connection = false;
		wait_us(4);
//		if(i2cWrite(&BTire)) connection = false;
//		wait_us(4);
		if(i2cWrite(&AirCylinder)) connection = false;
		wait_us(4);
		if(i2cWrite(&FServo)) connection = false;
		wait_us(100);
		if(i2cWrite(&BServo)) connection = false;

		LED(1,connection);

		Sleep(25);
	}	
}

