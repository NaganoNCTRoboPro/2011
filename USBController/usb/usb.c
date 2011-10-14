#include "usb.h"
#include <sl811.h>
#include <sony_sixaxis.h>
#include <led.h>

SWord __usb_dev_mode;
USBIntQueueItem *usbqueue;
volatile SWord usb_int_tx_enable;
CfgDesc	config;
Byte __device_addr[MAX_DEVICE_NUM];
USBIntQueueItem _newitem;

/**
 * USB Initialize
 * @return 正常終了: 0
 *         異常終了: -1
 */
SWord USBInit(void)
{
	SWord x;
	
	__usb_dev_mode = USB_NONE;
	
	x = SL811USBInit();
	
	return x;
}

/**
 * USB SWorderrupt Transfer Manager Initialize
 */
void USBIntTxManagerInit(void)
{
	usbqueue = NULL;
	usb_int_tx_enable = 1;
}

/**
 * USBデバイスの初期化処理
 *
 * @param {d} 対象デバイス
 * @param {parent} 親デバイス
 * @param {speed} 対象デバイスの速度
 * @return 正常終了時 : 0
 *         不正終了時 : -1
 */
SWord USBDeviceInit_in3(USBDevice *d, USBDevice *parent, SWord speed)
{
	SWord devaddr;
	
//**/	SWord	i, epnum;
	Byte tmp;
	
	// EndPoSWord No. - 0, EndPoSWordSize - 8, Retry - 12, PRE - Preamble, Address - 0
	SL811DeviceInit(&(d->devinfo), 0, 8, 12 + ((parent != NULL && speed == USB_LOW) ? 8 : 0), ((parent != NULL && speed == USB_LOW) ? 1 : 0), 0);
	d->speed = speed;
	
	// デバイスディスクプリタの仮取得
	if( Get_Descriptor(d, DEVICE_TYPE, &(d->devdesc), 8, 0, 0) < 0 )	return -1;
LED2_ON;	
	// エンドポイントサイズの設定
	SL811DeviceInit(&(d->devinfo), 0, d->devdesc.bMaxPacketSize0, 20, 0, 0);
	
	// デバイスアドレスをセット
	devaddr = 1;
	if( Set_Address(d, devaddr) == -1 )	return -1;
	
	// デバイスディスクプリタの取得
	if( Get_Descriptor(d, DEVICE_TYPE, &(d->devdesc), sizeof(DevDesc), 0, 0) < 0 )	return -1;

	// コンフィグレーションディスクプリタの仮取得
	if( Get_Descriptor(d, CONFIG_TYPE, &config, sizeof(CfgDesc), 0, 0) < 0 )	return -1;

	// コンフィグレーションディスクプリタの取得
	d->cfgdesc = &config;
	if( Get_Descriptor(d, CONFIG_TYPE, d->cfgdesc, config.wTotalLength, 0, 0) < 0 )	return -1;
LED2_OFF;	
	// ポインタのセット
	d->intfdesc = (IntfDesc*)((Byte*)d->cfgdesc + d->cfgdesc->bLength);

	if( d->devdesc.bDeviceClass == 0x00 &&
		d->intfdesc->bInterfaceClass == HID_CLASS ){
		
		d->hiddesc = (HIDDesc*)((Byte*)d->intfdesc + d->intfdesc->bLength);
		d->epdesc = (EPDesc*)((Byte*)d->hiddesc + d->hiddesc->bLength);

		// レポートディスクリプタの取得
		if( d->hiddesc->Report[0].bDescriptorType == REPORT_TYPE )
		{
//			d->repdesc = &_repdesc;//(RptDesc*)malloc( (d->hiddesc->Report[0].wDescriptorLength) );
//			Get_Descriptor(d, REPORT_TYPE, d->repdesc, d->hiddesc->Report[0].wDescriptorLength, 0, 0);
		}
	}else{
		d->hiddesc = NULL;
		d->repdesc = NULL;
		d->epdesc = (EPDesc*)((Byte*)d->intfdesc + d->intfdesc->bLength);
	}

	// コンフィギュレーションの設定
	Set_Configuration(d, d->cfgdesc->bConfigurationValue);
	
	// 情報の表示
	
	Get_Configuration(d, &tmp);
//**/	if( tmp != 0x00 ) 	DBG("Configuration Finished!\n");
/*
	DBG("          ---===>Device Information<===---\n");

	DBG("\nConnectionStatus:\n");
	DBG("  -> Device Bus Speed: ");
	DBG( (d->speed==USB_FULL) ? "Full\n" : "Low\n");
	DBG("Device Address:       0x%02X\n", d->address);

	prSWord_desc(&d->devdesc, DEVICE_TYPE);
	prSWord_desc(d->cfgdesc, CONFIG_TYPE);
	prSWord_desc(d->SWordfdesc, SWordERFACE_TYPE);
	
	if( d->devdesc.bDeviceClass == 0x00 &&
		d->SWordfdesc->bSWorderfaceClass == HID_CLASS ){
		prSWord_desc(d->hiddesc, HID_TYPE);
	}

	epnum = d->SWordfdesc->bNumEndpoSWords;

	for( i=0; i<epnum; i++){
		prSWord_desc(&d->epdesc[i], ENDPOSWord_TYPE);
	}	
*/
	return 0;
}

/**
 * SL811 Device Initialize
 *
 * @param {d} 対象デバイス
 * @param {ep} エンドポイント番号
 * @param {epsize} エンドポイントのサイズ
 * @param {retry} リトライ回数
 * @param {pre} プリアンブルパケットの有無
 * @param {sddr} デバイスのアドレス
 */
void SL811DeviceInit(SL811USBDevInfo *d,
					 SWord ep, 
					 SWord epsize, 
					 SWord retry, 
					 SWord pre, 
					 SWord addr){
	d->ep = ep;
	d->epsize = epsize;
	d->retry = retry;
	d->pre = (pre) ? (PREAMBLE) : (0x00);
	d->addr = addr;
}


/**
 * Get Descriptor
 *
 * @param {d} 対象デバイス
 * @param {type} ディスクリプタの種類
 * @param {data} 受信するデータ
 * @param {length} 受信するデータのサイズ
 * @param {index} index値 USB仕様書 Get Descriptor参照
 * @param {lang}lang値 USB仕様書 Get Descriptor参照
 * @return 正常終了時 : 受信したデータのサイズ
 *         不正終了時 : HANDSHAKE_NAK [-1] もしくは HANDSHAKE_STALL [-2]
 */
SWord Get_Descriptor(USBDevice *d,
					 Byte type, 
					 void *data, 
					 SWord length, 
					 Byte index, 
					 SWord lang){
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_HOST;
	
	if( type == HID_TYPE || type == REPORT_TYPE )	command.bmRequestType |= TRANS_TO_IF;

	command.bRequest = GET_DESCRIPTOR;
	command.wValue = (type << 8) | index;
	command.wIndex = lang;
	command.wLength = length;
	
	return CtrlTransfer(d, &command, (Byte*)data);
}

/**
 * USB Control Transfer
 *
 * @param {d} 対象デバイス
 * @param {command} リクエスト
 * @param {data} 送信／受信するデータ
 * @return 正常終了時 : 送信／受信したデータのサイズ
 *         不正終了時 : HANDSHAKE_NAK [-1] もしくは HANDSHAKE_STALL [-2]
 */
SWord CtrlTransfer(USBDevice *d, CtrlReq *command, Byte *data)
{
	d->devinfo.ep = 0;
	return SL811CtrlTransfer(&(d->devinfo), command, data);
}

/**
 * SL811 USB Control Transfer
 *
 * @param {d} 対象デバイス
 * @param {command} リクエスト
 * @param {data} 送信／受信するデータ
 * @return 正常終了時 : 送信／受信したデータのサイズ
 *         不正終了時 : SL811_HANDSHAKE_NAK [-1] もしくは SL811_HANDSHAKE_STALL [-2]
 */
SWord SL811CtrlTransfer(SL811USBDevInfo *d, CtrlReq *command, Byte *data)
{
	SWord size, ret, length;

	d->wr_cmd = DATA0_WR;
	size = SL811WriteData(d, (Byte*)command, sizeof(CtrlReq), SL811WRITE_SETUP);	// SETUP

	if (size < 0)	return size;
	
	length = command->wLength;

	if( length != 0 && data != NULL ){
		if( command->bmRequestType & TRANS_TO_HOST ){
			// Read Control
			d->rd_cmd = DATA1_RD;
			size = SL811ReadData(d, data, length);			// IN(DATA)
			if (size < 0)	return size;
			ret = size;

			d->wr_cmd = DATA1_WR;
			size = SL811WriteData(d, NULL, 0, SL811WRITE_OUT);			// OUT(STATUS)
		}else{
			// Write Control
			d->wr_cmd = DATA1_WR;
			size = SL811WriteData(d, data, length, SL811WRITE_OUT);		// OUT(DATA)
			if (size < 0)	return size;
			ret = size;

			d->rd_cmd = DATA1_RD;
			size = SL811ReadData(d, NULL, 0);				// IN(STATUS)
		}
	}else{
		// No-data Control
		d->rd_cmd = DATA1_RD;
		size = SL811ReadData(d, NULL, 0);					// IN(STATUS)
		ret = 0;
	}
	if (size < 0)	return size;
	return ret;
}

/**
 * Set Device Address
 *
 * @param {d} 対象デバイス
 * @param {addr} アドレス
 * @return 正常終了時 : 受信したデータのサイズ
 *         不正終了時 : HANDSHAKE_NAK [-1] もしくは HANDSHAKE_STALL [-2]
 */
SWord Set_Address(USBDevice *d, SWord addr)
{
	CtrlReq command;
	SWord ret;

	command.bmRequestType = TRANS_TO_DEV;
	command.bRequest = SET_ADDRESS;
	command.wValue = addr;
	command.wIndex = 0;
	command.wLength = 0;
	
	
	ret = CtrlTransfer(d, &command, NULL);
	if( ret<0 )	return ret;
	
	__device_addr[addr] = 1;
	d->address = addr;
	SL811SetAddress(&(d->devinfo), addr);

	return ret;
}


/**
 * Set Configuration
 *
 * @param {d} 対象デバイス
 * @param {config} コンフィギュレーション番号
 * @return 正常終了時 : 送信したデータのサイズ
 *         不正終了時 : HANDSHAKE_NAK [-1] もしくは HANDSHAKE_STALL [-2]
 */
SWord Set_Configuration(USBDevice *d, SWord config)
{
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_DEV;
	command.bRequest = SET_CONFIG;
	command.wValue = config;
	command.wIndex = 0;
	command.wLength = 0;

	return CtrlTransfer(d, &command, NULL);
}

/**
 * Get Configuration
 *
 * @param {d} 対象デバイス
 * @param {config} コンフィギュレーション番号
 * @return 正常終了時 : 受信したデータのサイズ
 *         不正終了時 : HANDSHAKE_NAK [-1] もしくは HANDSHAKE_STALL [-2]
 */
SWord Get_Configuration(USBDevice *d, Byte *config)
{
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_HOST;
	command.bRequest = GET_CONFIG;
	command.wValue = 0;
	command.wIndex = 0;
	command.wLength = 1;

	return CtrlTransfer(d, &command, config);
}




/**
 * @param {d} 対象デバイス
 * @param {type} Report Type
 * @param {data} 受信するデータ
 * @param {length} 受信するデータのサイズ
 * @param {report_id} Report ID
 * @param {SWordf} インターフェース番号
 * @return 正常終了時 : 受信したデータのサイズ
 *         不正終了時 : HANDSHAKE_NAK [-1] もしくは HANDSHAKE_STALL [-2]
 */
SWord Get_Report(USBDevice *d, SWord type, Byte *data, size_t length, Byte report_id, SWord intf)
{
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_HOST | TRANS_TO_IF | TYPE_CLASS;
	command.bRequest = GET_REPORT;
	command.wValue = ((type << 8) | report_id);
	command.wIndex = intf;
	command.wLength = length;

	return CtrlTransfer(d, &command, data);
}

/**
 * USB Add SWorderrupt Transfers Task
 *
 * @param {d} 対象デバイス
 * @param {ep} エンドポイント番号（1～）
 * @param {SWorderval} インタラプト転送処理間隔
 * @param {fp} インタラプト転送処理ハンドラ
 * @return 正常終了時 : 0
 *         不正終了時 : -1
 */
SWord USBAddIntTxTask(USBDevice *d, SWord ep, SWord interval, void (*fp)(USBDevice*))
{
	
	USBIntQueueItem	*newitem;
	SWord i;
	char flag;
	
	if( ep < 1 || ep > 15 )	return -1;
	
	flag = 0;
	for( i=0; i < d->intfdesc->bNumEndpoints; i++ ){
		if( (d->epdesc[i].bEndpointAddress & EPADDR_MASK_ADDRESS) == ep ){
			flag = 1;
			break;
		}
	}
	if( !flag || (((d->epdesc[i].bmAttributes) & 0x03 ) != 3) ||  (d->epdesc[i].bInterval == 0) )	return -1;

	newitem = &_newitem;
	
	newitem->device = d;
	newitem->ep = ep;
	newitem->handler = fp;
	newitem->interval = interval;
	newitem->cnt = interval;
	
	newitem->next = usbqueue;
	usbqueue = newitem;	// ポインタのつなぎかえ

	return 0;
}


/**
 * USB interrupt IN Transfer
 *
 * @param {d} 対象デバイス
 * @param {ep} エンドポイント番号
 * @param {data} 受信するデータ
 * @param {size} 受信するデータのサイズ
 */
SWord IntInTransfer(USBDevice *d, SWord ep, Byte *data, SWord size)
{
	d->devinfo.ep = ep;
	return SL811IntInTransfer(&(d->devinfo), data, size);
}


/**
 * SL811 USB interrupt IN Transfer
 * 
 * @param 対象デバイス
 * @param 受信するデータ
 * @param 受信するデータのサイズ
 * @return 正常終了時 : 受信したデータのサイズ
 *         不正終了時 : SL811_HANDSHAKE_NAK [-1] もしくは SL811_HANDSHAKE_STALL [-2]
 */
SWord SL811IntInTransfer(SL811USBDevInfo *d, Byte *data, SWord size)
{
	d->rd_cmd = DATA0_RD;
	return SL811ReadData(d, data, size);
}
