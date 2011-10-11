#ifndef __USBHID_H__
#define __USBHID_H__

#include"sl811.h"
#include"usb.h"
#include"sl811hcd.h"
#include"usbhub.h"
#include"other.h"
#include"sony_sixaxis.h"

/// <summary>
/// HID Class Specific Requests
/// </summary>
#define	HID_TYPE			0x21
#define	REPORT_TYPE			0x22
#define	PHYSICAL_TYPE		0x23

/// <summary>
/// HID Protocol
/// </summary>
#define	BOOT_PROTOCOL		0
#define	REPORT_PROTOCOL		1

#define	RPT_TYPE_INPUT		0x01
#define	RPT_TYPE_OUTPUT		0x02
#define	RPT_TYPE_FEATURE	0x03

/// <summary>
/// HID bRequest Value
/// </summary>
#define	GET_REPORT			 0x01
#define	GET_IDLE			 0x02
#define	GET_PROTOCOL		 0x03
#define	SET_REPORT			 0x09
#define	SET_IDLE			 0x0a
#define	SET_PROTOCOL		 0x0b


/// <summary>
/// HID Descriptor
/// </summary>
typedef struct {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned short	bcdHID;
	unsigned char	bCountryCode;
	unsigned char	bNumDescriptors;

	struct {
		unsigned char	bDescriptorType;
		unsigned char	wDescriptorLength;	// Total size of the Report descriptor
	} __attribute__((packed)) Report[1];
} __attribute__ ((packed)) HIDDesc;

/// <summary>
/// Report Descriptor
/// </summary>
typedef struct {
	unsigned char bReport;
} __attribute__ ((packed)) RptDesc;

/// <summary>
/// Physical Descriptor
/// </summary>
typedef struct {
	unsigned char bPhysical;
} __attribute__ ((packed))  HidPhyDesc;

#endif
