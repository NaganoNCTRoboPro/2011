#include "actuater.h"
#include <i2c.h>
#include <common.h>

void mDrive(Slave *motor,unsigned char in, signed char duty,unsigned char no)
{
	signed char w_data=0;
	if((no>4)||(no<0)) return;
	if(duty>100) duty=100;
	if(duty<-100) duty=-100;
	if(in==CCW) w_data = -duty;
	else if(in==FREE) w_data = 0; 
	else if(in==BRAKE) w_data = 0x7f;
	else w_data = duty;
	motor->write.buffer[no] = w_data;
}

void mAngle(Slave *motor,int angle,bool act)
{
	if(act){
		angle/=6;
		motor->write.buffer[2] = 0x40|(angle&0x1f);
		}
	else motor->write.buffer[2] = 0x00;
}

void aDrive(Slave *cylinder,unsigned char port,bool act)
{
	if(act) *(cylinder->write.buffer) = port;
	else 	*(cylinder->write.buffer) = 0x00;
}

/*
void sDrive(Slave *servo,unsigned char angle,unsigned char number)
{
	*(servo->write.buf) = ((angle/6)<<3)|number;
}
*/
