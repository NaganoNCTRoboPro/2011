#include <avr/io.h>
#include "motor.h"

void m0_d1(bool x){if(x) sbi(PORTC,PC0);else cbi(PORTC,PC0);}
void m0_d2(bool x){if(x) sbi(PORTC,PC1);else cbi(PORTC,PC1);}
void m1_d1(bool x){if(x) sbi(PORTD,PD1);else cbi(PORTD,PD1);}
void m1_d2(bool x){if(x) sbi(PORTD,PD2);else cbi(PORTD,PD2);}
void m2_d1(bool x){if(x) sbi(PORTD,PD4);else cbi(PORTD,PD4);}
void m2_d2(bool x){if(x) sbi(PORTD,PD3);else cbi(PORTD,PD3);}
void m3_d1(bool x){if(x) sbi(PORTD,PD7);else cbi(PORTD,PD7);}
void m3_d2(bool x){if(x) sbi(PORTB,PB0);else cbi(PORTB,PB0);}

void(*d1)(bool);
void(*d2)(bool);

void m_act(uint8_t act,int target)
{
	switch(target){
		case 0:	d1=m0_d1;d2=m0_d2;break;
		case 1:	d1=m1_d1;d2=m1_d2;break;
		case 2:	d1=m2_d1;d2=m2_d2;break;
		case 3:	d1=m3_d1;d2=m3_d2;break;
		default: return;				
		}
	switch(act){
		case CW:	(*d1)(0);(*d2)(1);break;
		case CCW:	(*d1)(1);(*d2)(0);break;
		case FREE:	(*d1)(0);(*d2)(0);break;
		case BRAKE:	(*d1)(1);(*d2)(1);break;
		default: return;
		}
}
void m0(uint8_t act,uint8_t duty){m_act(act,0);OCR0A=duty;}
void m1(uint8_t act,uint8_t duty){m_act(act,1);OCR0B=duty;}
void m2(uint8_t act,uint8_t duty){m_act(act,2);OCR1A=duty;}
void m3(uint8_t act,uint8_t duty){m_act(act,3);OCR1B=duty;}
void (*motor[4])(uint8_t act,uint8_t duty)={m0,m1,m2,m3};

void setup()
{
	DDRC   = 0x33;
	DDRD   = 0xff;
	PORTD  = 0x00;
	DDRB   = 0xff;
	PORTB  = 0x00;
	TCCR0A = 0xA3;  
	TCCR0B = 0x03;                    
	TCCR1A = 0xA1;     
	TCCR1B = 0x0B;                    
}
