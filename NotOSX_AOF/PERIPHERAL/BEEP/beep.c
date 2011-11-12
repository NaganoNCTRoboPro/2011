#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <led.h>
#include "beep.h"

static bool ctrl=1;

void beep(bool act)
{
	if(BEEP&&act&&ctrl) PORTA|=0x08;
	else if(ctrl) PORTA&=0xf7;
}

void BeepInit()
{
	DDRA |= 0x08;
	PORTA &= 0xf7;
}

void i2cCheck(bool flag)
{
	LED(1,flag);
	beep(!(flag));
	if(!flag){
		PORTC = 0x04;
		cli();
		while(1);
	}
}



















