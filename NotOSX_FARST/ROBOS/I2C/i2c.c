/*
 * i2c.c
 *
 *  Created on: 2011/7/25
 *      Author: iwate
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <led.h>
#include <beep.h>
#include <wait.h>
#include "i2c.h"

#if WDT_RESET_IN_I2C
	#include <avr/wdt.h>
#endif

#if MASTER_COMPILE
	volatile static unsigned char i;
#endif

#if SLAVE_COMPILE
	static Slave* own;
	volatile static uint8_t count = 0;
	volatile bool emergency = false;
	volatile bool i2cComFlag = false;
#endif

#if MASTER_COMPILE
void initI2CMaster(unsigned char speed)
    {
		TWBR = (((CLOCK_FREQ*1000)/speed)-16)/2;
		TWSR = 0;
		TWCR = I2CEN;
    }
#endif

#if SLAVE_COMPILE
void initI2CSlave(Slave *_own)
	{
		own = _own;
		TWAR = (own->addr)>>=1;
		TWAR |= 1;
		TWCR = 0x45;
	#if WDT_RESET_IN_I2C
		wdt_reset();
		wdt_enable(WDT_RESET_TIME);
	#endif
	}
#endif

#if MASTER_COMPILE
bool i2cWrite(Slave *slave)
    {
//			cli();

#if WDT_RESET_IN_I2C
		wdt_reset();
		wdt_enable(WDT_RESET_TIME);
#endif
		TWCR = (1<<TWINT)|(1<<TWSTA) |(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x08) goto ERROR;

		TWDR = (slave->addr<<1);
		TWCR = (1<<TWINT)|(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x18) goto ERROR;

		for(i=0;i<slave->write.size;i++){
			TWDR=slave->write.buf[i];
			TWCR=(1<<TWINT)|(1<<TWEN);
			while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
			if((TWSR&0xF8)!=0x28) goto ERROR;
			}
		TWCR=(1<<TWINT)|(1<<TWSTO) |(1<<TWEN);
#if WDT_RESET_IN_I2C
		wdt_disable();
#endif
//		sei();
		return true;
	ERROR:
		TWCR=(1<<TWINT)|(1<<TWSTO) |(1<<TWEN);
//		sei();
#if WDT_RESET_IN_I2C
		wdt_disable();
#endif
		return false;
    }

bool i2cRead(Slave *slave)
    {
//		cli();

		TWCR = (1<<TWINT)|(1<<TWSTA) |(1<<TWEN);
#if WDT_RESET_IN_I2C
		wdt_reset();
		wdt_enable(WDT_RESET_TIME);
#endif
		while(!(TWCR&(1<<TWINT)));
		if((TWSR&0xF8)!=0x08) goto ERROR;

		TWDR = (slave->addr<<1)|0x01;
		TWCR = (1<<TWINT)|(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x40) goto ERROR;

		for(i=0;i<slave->read.size-1;i++){
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
			while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
			wdt_reset();
#endif
			if((TWSR&0xF8)!=0x50) goto ERROR;
			slave->read.buf[i] = TWDR;
			}
		TWCR = (1<<TWINT)|(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x58) goto ERROR;
		slave->read.buf[i] = TWDR;

		TWCR=(1<<TWINT)|(1<<TWSTO) |(1<<TWEN);
//		sei();
#if WDT_RESET_IN_I2C
		wdt_disable();
#endif
		return true;
	ERROR:
		TWCR=(1<<TWINT)|(1<<TWSTO) |(1<<TWEN);
//		sei();
#if WDT_RESET_IN_I2C
		wdt_disable();
#endif
		return false;
    }

bool i2cReadWithCommand(Slave *slave, unsigned char command)
    {
//		cli();

		TWCR = (1<<TWINT)|(1<<TWSTA) |(1<<TWEN);
#if WDT_RESET_IN_I2C
		wdt_reset();
		wdt_enable(WDT_RESET_TIME);
#endif
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
//		if((TWSR&0xF8)!=0x08) goto ERROR;

		TWDR = (slave->addr<<1);
		TWCR = (1<<TWINT)|(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x40) goto ERROR;

		TWDR=command;
		TWCR=(1<<TWINT)|(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x28) goto ERROR;

		TWCR = (1<<TWINT)|(1<<TWSTA) |(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x10) goto ERROR;

		TWDR = (slave->addr<<1)|0x01;
		TWCR = (1<<TWINT)|(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
//		if((TWSR&0xF8)!=0x40) goto ERROR;

		for(i=0;i<slave->read.size-1;i++){
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
			while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
			wdt_reset();
#endif
			if((TWSR&0xF8)!=0x50) goto ERROR;
			slave->read.buf[i] = TWDR;
			}
		TWCR = (1<<TWINT)|(1<<TWEN);
		while(!(TWCR&(1<<TWINT)));
#if WDT_RESET_IN_I2C
		wdt_reset();
#endif
		if((TWSR&0xF8)!=0x58) goto ERROR;
		slave->read.buf[i] = TWDR;

		TWCR=(1<<TWINT)|(1<<TWSTO) |(1<<TWEN);
//		sei();
#if WDT_RESET_IN_I2C
		wdt_disable();
#endif
		return true;
	ERROR:
		TWCR=(1<<TWINT)|(1<<TWSTO) |(1<<TWEN);
//		sei();
#if WDT_RESET_IN_I2C
		wdt_disable();
#endif
		return false;
    }
#endif



#if SLAVE_COMPILE

ISR (TWI_vect)
	{
	    switch(TWSR)                            
			{
				case 0x60:						
					count = 0;
					TWCR |= 0x80;	
					i2cComFlag = true;
					#if WDT_RESET_IN_I2C
						wdt_reset();
					#endif
					break;
	            case 0x90:
	                emergency = true;
				case 0x80:                      
					*(own->write.buf+count) = TWDR;	   
					count++;
					TWCR |= 0x80;		
					break;
				case 0xA0:
					TWCR |= 0x80;
					break;				
	            case 0xA8:
					i2cComFlag = true;
					count = 0;
					TWDR = *own->read.buf;				
					count++;					
					TWCR |= 0x80;
					#if WDT_RESET_IN_I2C
						wdt_reset();
					#endif				
					break;
				case 0xB8:						
					TWDR = *(own->read.buf + count);	
					count++;					
					TWCR |= 0x80;				
					break;
			    case 0x70:                      
	                *own->read.buf = 0;
				case 0xC0:   
					TWCR |= 0x80;
					i2cComFlag = false;
					break;					
				default:
					TWCR |= 0x80;
					i2cComFlag = false;				
					break;
			}
	}
#endif

