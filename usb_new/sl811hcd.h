#ifndef __SL811HCD_H__
#define __SL811HCD_H__

#include"sl811.h"
#include"usb.h"
#include"usbhid.h"
#include"usbhub.h"
#include"other.h"
#include"sony_sixaxis.h"

typedef struct {
	unsigned char	addr;		// USBデバイスアドレス
	unsigned char	ep;			// エンドポイント番号
	unsigned char	epsize;		// エンドポイントのサイズ
	unsigned char	retry;		// リトライ回数
	unsigned char	wr_cmd;		// 送信用パケットのトグル
	unsigned char	rd_cmd;		// 受信用パケットのトグル
	unsigned char	pre;		// PREパケットの有無
} SL811USBDevInfo;

#define SL811WRITE_OUT		0
#define SL811WRITE_SETUP	1

#define SL811_USB_NONE		0
#define SL811_USB_LOW		1
#define SL811_USB_FULL		2

#define SL811_HANDSHAKE_NAK		(-1)
#define SL811_HANDSHAKE_STALL	(-2)

#endif
