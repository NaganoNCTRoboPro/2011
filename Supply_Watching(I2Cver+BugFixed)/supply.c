#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "supply.h"

void init_adc()
{
	ADMUX=0b01000000;
	ADCSRA=0b10101110;
	ADCSRB=0b00000000;
}

void init_INT0()
{
	EICRA=0b00000100;
	EIMSK=0b00000001;
}

void setup()
{
	DDRC=0xca;
	DDRD=0xfb;
	DDRB=0xff;
	PORTD=0x04;
	PORTD |= 0xC0;
	PORTB |= 0x07;
	wdt_reset();
	wdt_enable(WDTO_500MS);
	cbi(PORTD,6);	
	cbi(PORTC,0);
	init_adc();
	init_INT0();
	sbi(PORTC,1);
	sbi(ADCSRA,ADSC);
}

void wait(uint16_t ms)
{          
	uint16_t i;
	TCCR2B= 0x03;       
	for( i=0;i<ms;i++ ){                          
		while( bit_is_clear(TIFR2,TOV2));               
		TIFR2  = _BV(TOV2); 
		}
}
