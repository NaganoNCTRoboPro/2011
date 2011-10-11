#ifndef __RC_CLASSIC_H__
#define __RC_CLASSIC_H__

#define CLASSIC_DATA_LENGTH	4

union classic_data {
	struct {
		struct {
			unsigned short X:1;
			unsigned short A:1;
			unsigned short B:1;
			unsigned short Y:1;
			unsigned short UP:1;
			unsigned short RIGHT:1;
			unsigned short DOWN:1;
			unsigned short LEFT:1;

			unsigned short SELECT:1;
			unsigned short START:1;
			unsigned short ZL:1;
			unsigned short ZR:1;
			unsigned short L:2;
			unsigned short R:2;
		}  __attribute__ ((packed)) Button;
		struct {
			unsigned char Y:4;
			unsigned char X:4;
		} __attribute__ ((packed)) AnalogL;
		struct {
			unsigned char Y:4;
			unsigned char X:4;
		} __attribute__ ((packed)) AnalogR;
	} __attribute__ ((packed)) detail;

	unsigned char buf[CLASSIC_DATA_LENGTH];
};
#define RC_DEFAULT_DATA	{0x00, 0x00, 0x00, 0x77, 0x77, 0x00, 0x00}

#endif
