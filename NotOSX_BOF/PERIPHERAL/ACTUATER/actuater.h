#ifndef __ACTUATER_H__
#define __ACTUATER_H__

#include <i2c.h>

#define FREE	(0x00)
#define CW		(0x01)
#define CCW		(0x02)
#define BRAKE	(CW | CCW)
#define ON 		(1)
#define OFF 	(0)

void mDrive(Slave *motor,unsigned char in, signed char duty,unsigned char no);
//void sDrive(Slave *servo,unsigned char angle,unsigned char number);
void aDrive(Slave *cyrinder,unsigned  char port,bool act);
void mAngle(Slave*motor,int angle,bool act);

#define MotorDrive(w,x,y,z) mDrive(w,x,y,z)
//#define ServoDrive(x,y,z) sDrive(x,y,z)
#define AirCylinderDrive(x,y,z) aDrive(x,y,z)
#define tDrive(x,y,z) aDrive(x,y,z);
#define ThrowDrive(x,y,z) aDrive(x,y,z);
#define MotorDriveAngle(x,y,z) mAngle(x,y,z)

#endif
