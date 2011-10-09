#ifndef __RC_NUNCHUK_H__
#define __RC_NUNCHUK_H__

#define NUNCHUK_DATA_LENGTH	3

union nunchuk_data {
	struct {
		struct {
			unsigned char  :6;
			unsigned char C:1;
			unsigned char Z:1;		
		}  __attribute__ ((packed)) Button;		//�{�^��
		struct {
			unsigned char Y:4;
			unsigned char X:4;
		} __attribute__ ((packed)) Analog;		//�A�i���O�X�e�B�b�N
	struct {
			unsigned char Y:3;
			unsigned char X:3;
			unsigned char Z:2;
		} __attribute__ ((packed)) Acceleration;//�����x
	} __attribute__ ((packed)) detail;
	unsigned char buf[NUNCHUK_DATA_LENGTH];
};

#endif