#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <led.h>
#include "beep.h"

#define FALL_COUNT (10)

static bool ctrl=1;
static uint8_t count = 0;

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
		count++;
	}else{
		count = 0;
	}
	if(count>=FALL_COUNT){
		count = 0;
		PORTC = 0x04;
		cli();
		while(1);
	}
}
