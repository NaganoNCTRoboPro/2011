#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "i2c.h"
#include "supply.h"

static int8_t buf[2]={0};
Slave data = {ADDR,{(int8_t*)&buf[0],1},{(int8_t*)&buf[1],1}};
volatile uint16_t adc_value=0;
volatile uint8_t cnt=1,w_cnt=0;

int main(void)
{
	initI2CSlave(&data);
	setup();
	sei();
	while(1){
		if(*(data.write.buf)==0x6A) Emergency_Stop();
		wait(25);
		}
	return 0;
}

void Emergency_Stop()
{
	sei();
	*(data.read.buf)=0x00;
	while(1){
		*(data.read.buf)=0x6A;
		cbi(PORTC,1);
		sbi(PINB,0);
		sbi(PINB,1);
		sbi(PINB,2);
		sbi(PIND,7);
		if(*(data.write.buf)==0x55){
			*(data.read.buf)=0x55;
			sbi(PORTC,1);
			sbi(PORTB,0);
			sbi(PORTB,1);
			sbi(PORTB,2);
			sbi(PORTD,7);
			while(i2cComFlag);
			while(!i2cComFlag);
			WDTCSR |= 0x08;
			WDTCSR &= 0xBF;
			wdt_reset();
			wdt_enable(WDTO_15MS);
			while(1);
			}
		wait(250);
		}
}
ISR(ADC_vect)
{	
	adc_value=ADC+adc_value;  
	if(cnt==10){
		adc_value=adc_value/10;
		if(510+(5*(Ampere)*0.9)<=adc_value){
			wait(500);
            uint8_t i;
           for(i=0;i<10;i++){
				adc_value=0;
				loop_until_bit_is_set(ADCSRA,ADIF);
				adc_value=ADC;
            	if(510+(5*(Ampere)*0.9)<=adc_value) w_cnt++;
            	}
			if(7<=w_cnt) ;//Emergency_Stop();	
			}
        w_cnt=0;
		cnt=0;
		adc_value=0;
		}   	
	cnt++;
}	


ISR(INT0_vect)
{	
	if(bit_is_clear(PIND,PD2)){
		wait(50);
		if(bit_is_clear(PIND,PD2)){
			sbi(PORTB,4);
			Emergency_Stop();
			}
	}
}

