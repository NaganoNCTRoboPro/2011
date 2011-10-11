#ifndef __SONY_SIXAXIS_H__
#define __SONY_SIXAXIS_H__

#include"sl811.h"
#include"usb.h"
#include"sl811hcd.h"
#include"usbhid.h"
#include"usbhub.h"
#include"other.h"

//#ifdef STRUCT_USB_DEVICE
//int Sony_SIXAXIS_Init(USBDevice *d);
//#endif
//unsigned char *Toggle_SIXAXIS_Buffer(void);


#define SONY_SIXAXIS_VENDOR_ID       0x054c
#define SONY_SIXAXIS_PRODUCT_ID      0x0268

#define SIXAXIS_REPORT_ID		0xf2
#define SIXAXIS_REPORT_TYPE		0x03

#define SIXAXIS_DATA_LENGTH		48
#define SIXAXIS_REPORT_LENGTH	17

typedef struct {
	struct {
		unsigned long SELECT:1;
		unsigned long L3:1;
		unsigned long R3:1;
		unsigned long START:1;
		unsigned long UP:1;
		unsigned long RIGHT:1;
		unsigned long DOWN:1;
		unsigned long LEFT:1;

		unsigned long L2:1;
		unsigned long R2:1;
		unsigned long L1:1;
		unsigned long R1:1;
		unsigned long TRI:1;
		unsigned long CIR:1;
		unsigned long BAT:1;
		unsigned long SQU:1;
		
		unsigned long PS:1;
		unsigned long  :7;

		unsigned long  :8;	
	}  __attribute__ ((packed)) Button;
	struct {
		unsigned char X;
		unsigned char Y;
	} __attribute__ ((packed)) AnalogL;
	struct {
		unsigned char X;
		unsigned char Y;
	} __attribute__ ((packed)) AnalogR;
	unsigned char _noassign1[4];
	struct {
		unsigned char UP;
		unsigned char RIGHT;
		unsigned char DOWN;
		unsigned char LEFT;
		unsigned char L2;
		unsigned char R2;
		unsigned char L1;
		unsigned char R1;
		unsigned char TRI;
		unsigned char CIR;
		unsigned char BAT;
		unsigned char SQU;
	} __attribute__ ((packed)) AnalogButton;
	unsigned char _noassign2[15];
	struct {
		unsigned short X;		// ‰ºˆÊ10bit
		unsigned short Y;		// ‰ºˆÊ10bit
		unsigned short Z;		// ‰ºˆÊ10bit
	} Axis;
} __attribute__ ((packed)) SIXAXIS;

#endif
