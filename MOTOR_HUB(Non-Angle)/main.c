#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "motor.h"

#define ADDR 0x01

int main(void)
{
	int8_t i,duty[4],buf[12]={0},tmp[2]={0};
	int8_t act[4] = {0};
	int16_t d[4]={0};
	Slave data = {ADDR,{(int8_t*)&buf[0],4},{(int8_t*)&buf[4],4}};
	sei();
	setup();
	initI2CSlave(&data);
	while(1){
		for(i=0;i<4;i++) duty[i] = buf[i];
		for(i=0;i<4;i++){
			if(duty[i]==127)	act[i]=BRAKE;
			else if(duty[i]==0)	act[i]=FREE;
			else if(duty[i]>0)	act[i]=CW;
			else if(duty[i]<0) {act[i]=CCW;duty[i]=-duty[i];}
			d[i] = duty[i]*2;
			d[i] += duty[i]/10*5;
			d[i] += duty[i]/100*5;
			
			}
			for(i=3;i>=0;i--){
			motor[i](act[i],d[i]);
		}
		}
		
	return 0;
}
