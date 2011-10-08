/*
 * i2c.h
 *
 *  Created on: 2010/12/09
 *      Author: iwate
 */

#ifndef I2C_H
#define I2C_H

#include <stdbool.h>

typedef struct st_i2c_slave{
	unsigned char addr;
	struct {
		signed char *buf;
		unsigned size;
	}write;
	struct {
		signed char *buf;
		unsigned size;
	}read;
}Slave;

/*--------------read only---------------*/
#define I2CEN	(_BV(TWEN))
#define I2CINT	(_BV(TWEN)|_BV(TWINT))
/*--------------------------------------*/

/*--------------read write--------------*/
#define CLOCK_FREQ		8// [MHz]

#define WDT_RESET_IN_I2C (1)
#define WDT_RESET_TIME (WDTO_15MS)

#define MASTER_COMPILE	(0)
#define SLAVE_COMPILE	(1)
/*--------------------------------------*/


#if MASTER_COMPILE 
	void initI2CMaster(unsigned char speed);
	bool i2cWrite(Slave *slave);
	bool i2cRead(Slave *slave);
	bool i2cReadWithCommand(Slave *slave, unsigned char command);
#elif SLAVE_COMPILE
	void initI2CSlave(Slave *_own);
	extern volatile bool emergency;
	extern volatile bool i2cComFlag;
#endif

#define CONVERT(own) ((unsigned short)(&own))

#endif
