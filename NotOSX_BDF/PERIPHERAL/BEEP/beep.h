#ifndef __BEEP_H__
#define __BEEP_H__

#include <avr/io.h>
#include <stdbool.h>

#define BEEP (1)

void BeepInit(void);
void beep(bool act);
void i2cCheck(bool flag);

#endif
