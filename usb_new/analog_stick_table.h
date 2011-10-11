#ifndef __ANALOG_STICK_TABLE_H__
#define __ANALOG_STICK_TABLE_H__

const unsigned char analog_stick_table_l[32][32];
const unsigned char analog_stick_table_r[32][32];


#define	GET_STICK_X(x)	(x >> 4)
#define	GET_STICK_Y(y)	(y & 0x0f)


#endif
