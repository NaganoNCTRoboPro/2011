#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "wait.h"

#define TEAM_B_OFF (0)

#if TEAM_B_OFF
#define ADDR 0x50
#else
#define ADDR 0x40
#endif

int main(void)
{
	uint8_t buf[2]={0},tmp,tmps,pretmp=0;
	Slave data = {ADDR,{(int8_t*)&buf[0],1},{(int8_t*)&buf[1],1}};
	sei();
	initI2CSlave(&data);
	DDRB |= 0x07;	//LED0~2
	DDRD |= 0x0f;	//AIR0~3
    PORTD = 0x00;
	while(1){
		tmp   = buf[0];
		PORTB = tmp&0x07;	
		PORTD = tmp&0x0f;
		tmps=(tmp>>4);
		if(tmp){
			if(tmp!=pretmp){
				PORTD = tmps;
#if TEAM_B_OFF
				wait_ms(50);
				PORTD = 0x00;
				wait_ms(50);
				PORTD = tmp;
				wait_ms(50);
				PORTD = 0x00;
				wait_ms(50);
				PORTD = tmp;
				wait_ms(50);
				PORTD = 0x00;
				wait_ms(50);
				PORTD = tmp;
				wait_ms(50);
				PORTD = 0x00;
				wait_ms(50);
				PORTD = 0x00;
				wait_ms(50);
				PORTD = tmp;
				wait_ms(50);
				PORTD = 0x00;
				wait_ms(50);
				PORTD = tmp;
				wait_ms(50);
#else
				wait_ms(6);
#endif
				PORTD = 0x00;
				}
			}
		pretmp=tmp;
		}
	return 0;
}

