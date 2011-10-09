#ifndef __RC_SIXAXIS_H__
#define __RC_SIXAXIS_H__

#define RC_DATA_LENGTH	4

union controller_data {
	struct {
		struct {
			unsigned short LEFT:1;
			unsigned short DOWN:1;
			unsigned short RIGHT:1;
			unsigned short UP:1;
			unsigned short SQU:1;
			unsigned short BAT:1;
			unsigned short CIR:1;
			unsigned short TRI:1;
			unsigned short R1:1;
			unsigned short L1:1;
			unsigned short R2:2;
			unsigned short L2:2;
			unsigned short START:1;
			unsigned short SELECT:1;
		}  __attribute__ ((packed)) Button;
		struct {
			unsigned char X:4;
			unsigned char Y:4;
		} __attribute__ ((packed)) AnalogL;
		struct {
			unsigned char X:4;
			unsigned char Y:4;
		} __attribute__ ((packed)) AnalogR;
	} __attribute__ ((packed)) detail;

	unsigned char buf[RC_DATA_LENGTH];
};

#define RC_DEFAULT_DATA	{0x00, 0x00, 0x77, 0x77}
#endif
