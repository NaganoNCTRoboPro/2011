#ifndef __EMERGENCY_H__
#define __EMERGENCY_H__

#include <avr/io.h>
#include <stdbool.h>
#include <i2c.h>

#define E_ON 1
#define E_OFF 2
#define E_KEEP 0

bool Emergency(Slave *emergency,signed char act);
bool EmergencyStatus();

#endif
