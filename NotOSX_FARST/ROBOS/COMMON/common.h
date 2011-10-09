#ifndef __COMMON_H__
#define __COMMON_H__

#define PERIPHERAL_CLOCK_FREQ		20// [MHz]

#define UART_NUM		2

// utility definition
#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)

#ifndef NULL
#define NULL (void*)0
#endif

#endif
