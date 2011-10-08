#ifndef SUPPLY_H
#define SUPPLY_H

#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)
#define Ampere 100
#define ADDR 0x10

void init_adc();
void init_INT0();
void setup();
void wait(uint16_t ms);
void Emergency_Stop();

#endif
