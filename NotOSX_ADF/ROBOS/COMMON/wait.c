#include <avr/io.h>
void wait_ms(uint16_t t){
	uint8_t tt;
	TCCR2A = 0;
	TCCR2B = 1;
	OCR2A = 45;

	for(tt=100;t--;){
		while(tt--){
			for(TCNT2=0;!(TIFR2&0x02););
			TIFR2 |= 0x02;
		}
	}
}
void wait_us(uint16_t t){
	TCCR2A = 0;
	TCCR2B = 1;
	OCR2A = 1;
	t>>=1;
	while(t--){
		for(TCNT2=0;!(TIFR2&0x02););
		TIFR2 |= 0x02;
	}
}
