#include <avr/io.h>
#include <emergency.h>
#include <beep.h>
#include <avr/wdt.h>
#include <i2c.h>
#include <wait.h>

bool eflag;

bool Emergency(Slave *emergency,signed char act)
{
	bool tmp;
	if(act==E_ON){
		*(emergency->write.buf) = 0x6A;
		beep(1);
		eflag=1;
		}
	else if(act==E_OFF){
		*(emergency->write.buf) = 0x55;
		beep(0);
		eflag=0;
		}
	else *(emergency->write.buf) = 0x00;
	
	tmp = i2cRead(emergency);
	if(*(emergency->read.buf)==0x6A){
		beep(1);
		eflag=1;
		}
	if(*(emergency->read.buf)==0x55){
//		beep(0);
		WDTCSR |= 0x08;
		WDTCSR &= 0xBF;
		wdt_reset();
		wdt_enable(WDTO_15MS);
		while(1);
		}
	return tmp;
}

bool EmergencyStatus()
{
	if(eflag) return 0;
	else return 1;
}
