#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "motor.h"

#define ADDR 0x01

int main(void)
{
	uint8_t i=0;
	int8_t act[4]={0,0,0,0},buf[4]={0,0,0,0};
	int16_t duty[4]={0,0,0,0},d=0;
	Slave data = {ADDR,{&buf[0],4},{&buf[0],4}};
	setup();
	initI2CSlave(&data);
	sei();
	while(1)
		{
			for(i=0;i<4;i++)
				{
					duty[i] = buf[i];
				}
			for(i=0;i<4;i++)
				{
					if(duty[i]==127)
						{
							act[i]=BRAKE;
						}
					else if(duty[i]==0)
						{
							act[i]=FREE;
						}
					else if(duty[i]>0)
						{
							act[i]=CW;
						}
					else if(duty[i]<0)
						{
							act[i]=CCW;
							duty[i]=-duty[i];
						}
					d = duty[i]*2;
					d += duty[i]/10*5;
					d += duty[i]/100*5;
					duty[i] = d;
				}
			for(i=0;i<4;i++){
					motor[i](act[i],duty[i]);
				}
		}
	return 0;
}
