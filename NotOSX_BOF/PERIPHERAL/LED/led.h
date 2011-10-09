#ifndef __LED_H__
#define __LED_H__

#include <avr/io.h>
#include <stdbool.h>


void LEDInit(void);

/// <summary>
/// LED‘€ìŠÖ” 
/// </summary>
/// <param name="target">  </param>
/// <param name="status"> 0->Á“”@1->“_“” </param>
void LED(unsigned char target, bool status);

void ERROR_LED(void);

#endif
