#ifndef __COMMON_H__
#define __COMMON_H__
#define F_CPU 8000000UL
#include <util/delay.h>
#define PERIPHERAL_CLOCK_FREQ		8// [MHz]


#define UART_NUM		2

// utility definition
#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)

//#define NULL (void*)0

#endif
