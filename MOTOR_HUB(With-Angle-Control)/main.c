#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "motor.h"
#include <avr/interrupt.h>

#define ADDR 0x01

#define MARGIN   2
#define CLOSE   35
#define C_SPEED  5
#define D_SPEED 12

#define KP (70)
#define KI (10)
#define KD (6)

volatile int8_t buf[10]={0};
volatile Slave data = {ADDR,{(int8_t*)&buf[0],3},{(int8_t*)&buf[5],5}};

int main(void)
{
	int16_t i=0,act=0x00,duty[4],tmp,angle[2]={0},target=90,_target=90,diff[4]={0,0,0,0};
	int16_t P=0,I=0,D=0,sum,Angle0[2] = {-10,-10},Angle90[2] = {90,90};
	sei();
	setup();
	initI2CSlave(&data);
	sei();
	while(1){
		for(i=0;i<3;i++) duty[i] = buf[i];
		for(i=0;i<2;i++){
			if(duty[i]==0x7f)	act=BRAKE;
			else if(duty[i]==0)	act=FREE;
			else if(duty[i]>0)	act=CW;
			else if(duty[i]<0){
				act=CCW;
				duty[i]=-duty[i];
				}
			motor[i+2](act,duty[i]);
		}		
		for(i=0;i<2;i++){
			target = (duty[2]&0x1f)*6;
			if(target==0){
				target = Angle0[i];
			}else{
				target = Angle90[i];
			}
			angle[i] = get_angle(i+2);
			if( target - _target > 45 ) { 
					target = _target + 45;
				}
			else if( target - _target < -45 ){
				target = _target - 45;
			}
			if(target>180) target = 180;
			if(target<0)   target = 0;

			diff[i]  = target - angle[i];
			P  =   KP * diff[i] / 100;
			I += ( KI * ( ( diff[i] + diff[i+2] ) / 2 ) ) / 1000 ;
			D  = ( KD * ( diff[i]-diff[i+2] ) ) / 1000;
			diff[i+2] = diff[i];

			sum = P+I+D;

			if ( sum > 100 ) sum = 100;
			else if(sum < -100 ) sum = -100;

			if( sum > 0 ) act = CCW;
			else if( sum < 0 ) act = CW;

			tmp = (sum<0)? -sum: sum ;

			if( diff[i] < MARGIN && diff[i] > (-MARGIN ) ){
					 act=BRAKE;
					 I=0;
					 _target = target;
				 }

			motor[i](act,tmp);
		}

	}
	return 0;
}

