#ifndef __SL811HCD_H__
#define __SL811HCD_H__

#include"sl811.h"
#include"usb.h"
#include"usbhid.h"
#include"usbhub.h"
#include"other.h"
#include"sony_sixaxis.h"

typedef struct {
	unsigned char	addr;		// USB�f�o�C�X�A�h���X
	unsigned char	ep;			// �G���h�|�C���g�ԍ�
	unsigned char	epsize;		// �G���h�|�C���g�̃T�C�Y
	unsigned char	retry;		// ���g���C��
	unsigned char	wr_cmd;		// ���M�p�p�P�b�g�̃g�O��
	unsigned char	rd_cmd;		// ��M�p�p�P�b�g�̃g�O��
	unsigned char	pre;		// PRE�p�P�b�g�̗L��
} SL811USBDevInfo;

#define SL811WRITE_OUT		0
#define SL811WRITE_SETUP	1

#define SL811_USB_NONE		0
#define SL811_USB_LOW		1
#define SL811_USB_FULL		2

#define SL811_HANDSHAKE_NAK		(-1)
#define SL811_HANDSHAKE_STALL	(-2)

#endif
