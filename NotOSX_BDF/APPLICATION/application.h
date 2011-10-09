#ifndef APPLICATION_H
#define APPLICATION_H

#include <UserTask.h>
#include <i2c.h>

extern __inline__ void ApplicationInit(void);

__inline__ void ApplicationInit(void){
	UserTaskInit();
//	initI2CMaster(100);
}

#endif
