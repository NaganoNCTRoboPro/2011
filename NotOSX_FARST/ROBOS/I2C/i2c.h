#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdbool.h>

/**
 * config defines
 */
#define FREQ		8// [MHz]

#define MASTER_COMPILE	(1)
#define SLAVE_COMPILE	(0)

    // wdt is non suport
#define WDT_RESET_IN_I2C (1)        
#define WDT_RESET_TIME (WDTO_15MS)

/**
 * スレーブクラスの属性
 */
typedef struct st_i2c_slave {
	uint8_t address;
	struct {
		uint8_t* buffer;
		uint8_t size;
	} write;
	struct {
		uint8_t* buffer;
		uint8_t size;
	} read;
} Slave;

/**
 * プロトタイプ宣言
 */
#if MASTER_COMPILE 
	void initI2CMaster(uint16_t speed);
	bool i2cWrite(Slave* slave);
	bool i2cRead(Slave* slave);
	bool i2cReadWithCommand(Slave* slave, uint8_t command);
#elif SLAVE_COMPILE
	void initI2CSlave(Slave* _own);
	bool isEmergency(void);
	bool isCommunicatingViaI2C(void);
#else
    #warning I2C CONFIG IS NOT COMPILE
#endif // プロトタイプ宣言

#endif // I2C_H
