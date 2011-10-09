#ifndef __SWITCH_H__
#define __SWITCH_H__

#include <avr/io.h>
#include "common.h"
#include <stdbool.h>

#define SW_ON	true
#define SW_OFF	false

void SwitchInit(void);
bool Switch(unsigned char target);
unsigned char Group(void);
unsigned char Channel(void);

#endif
