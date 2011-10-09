#ifndef __RC_NUNCHUK_H__
#define __RC_NUNCHUK_H__

#define RC_DATA_LENGTH	3

union controller_data {
	struct {
		struct {
			unsigned char  :6;
			unsigned char C:1;
			unsigned char Z:1;		
		}  __attribute__ ((packed)) Button;		//ボタン
		struct {
			unsigned char Y:4;
			unsigned char X:4;
		} __attribute__ ((packed)) Analog;		//アナログスティック
		struct {
			unsigned char Y:3;
			unsigned char X:3;
			unsigned char Z:2;
		} __attribute__ ((packed)) Acceleration;//加速度
	} __attribute__ ((packed)) detail;
	unsigned char buf[RC_DATA_LENGTH];
};

#define RC_DEFAULT_DATA	{0x00, 0x77, 0x6E}

#endif
