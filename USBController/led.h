#ifndef LED_H
#define LED_H

#include <avr/io.h>

#define LED0_ON PORTD |= _BV(PORTD2)
#define LED1_ON PORTD |= _BV(PORTD4)
#define LED2_ON PORTD |= _BV(PORTD6)
#define LED_ON_ALL PORTD |= _BV(PORTD2) | _BV(PORTD4) | _BV(PORTD6)
#define LED0_OFF PORTD &= ~_BV(PORTD2)
#define LED1_OFF PORTD &= ~_BV(PORTD4)
#define LED2_OFF PORTD &= ~_BV(PORTD6)
#define LED_OFF_ALL PORTD &= ~(_BV(PORTD2) | _BV(PORTD4) | _BV(PORTD6))

#endif 
