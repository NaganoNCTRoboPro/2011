#include <avr/io.h>
#include <common.h>
#include <stdbool.h>
#include "led.h"


/// <summary>
/// LED‘€ìŠÖ” 
/// </summary>
/// <param name="target">  </param>
/// <param name="status"> 0->Á“”@1->“_“” </param>
void LED(unsigned char target, bool status)
{
	if(target > 2)return;
	if(status)	cbi(PORTA,target);
	else		sbi(PORTA,target);
}

void LEDInit(void)
{
	DDRA |= 0x07;
	PORTA |= 0x07;
}
