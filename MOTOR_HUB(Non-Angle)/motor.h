#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <avr/io.h>
#include <stdbool.h>

#define sbi(PORT,BIT) PORT|=_BV(BIT)
#define cbi(PORT,BIT) PORT&=~_BV(BIT)

#define CW    1
#define CCW   2
#define FREE  0
#define BRAKE 3

void m0(uint8_t act,uint8_t duty);
void m1(uint8_t act,uint8_t duty);
void m2(uint8_t act,uint8_t duty);
void m3(uint8_t act,uint8_t duty);
void (*motor[4])(uint8_t act,uint8_t duty);

void setup();

#endif
