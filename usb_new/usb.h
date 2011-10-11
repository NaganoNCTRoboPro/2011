#ifndef __USB_H__
#define __USB_H__

#include "sl811.h"
#include "sl811hcd.h"
#include "usbhid.h"
#include "usbhub.h"
#include "other.h"
#include"sony_sixaxis.h"

//#include "sl811hcd.h"
//#include "usbhub.h"

/// <summary>
/// USB Device Speed Status
/// </summary>
#define USB_NONE		0	// == SL811_USB_NONE (sl811hcd.h)
#define USB_LOW			1	// == SL811_USB_LOW  (sl811hcd.h)
#define USB_FULL		2	// == SL811_USB_FULL (sl811hcd.h)

/// <summary>
/// USB Handshake Status
/// </summary>
#define	HANDSHAKE_NAK	(-1)	// = SL811_HANDSHAKE_NAK   (sl811hcd.h)
#define HANDSHAKE_STALL	(-2)	// == SL811_HANDSHAKE_STALL (sl811hcd.h)


/// <summary>
/// bEndpointAddress bit mask
/// </summary>
#define EPADDR_MASK_DIRECTION	0x80
#define EPADDR_MASK_ADDRESS		0x0f


/// <summary>
/// Setup Data
/// </summary>
typedef struct {
	unsigned char	bmRequestType;
	unsigned char	bRequest;
	unsigned short	wValue;
	unsigned short	wIndex;
	unsigned short	wLength;
} __attribute__ ((packed)) CtrlReq;

/// <summary>
/// bmRequestType Value
/// </summary>
#define	TRANS_TO_DEV		0x00
#define	TRANS_TO_HOST		0x80
#define	TRANS_TO_IF			0x01
#define	TRANS_TO_EP			0x02
#define	TRANS_TO_OTHER		0x03
#define	TYPE_STD			0x00
#define	TYPE_CLASS			0x20
#define	TYPE_VENDOR			0x40

/// <summary>
/// bRequest Value
/// </summary>
#define	GET_STATUS			0x00
#define	CLEAR_FEATURE		0x01
#define	SET_FEATURE			0x03
#define	SET_ADDRESS			0x05
#define	GET_DESCRIPTOR		0x06
#define	SET_DESCRIPTOR		0x07
#define	GET_CONFIG			0x08
#define	SET_CONFIG			0x09
#define	GET_INTERFACE		0x0a
#define	SET_INTERFACE		0x0b
#define	SYNCH_FRAME			0x0c

/// <summary>
/// Type of Descriptor
/// </summary>
#define	DEVICE_TYPE			0x01
#define	CONFIG_TYPE			0x02
#define	STRING_TYPE			0x03
#define	INTERFACE_TYPE		0x04
#define	ENDPOINT_TYPE		0x05

/// <summary>
/// Device Class
/// </summary>
#define	STD_CLASS			0x00	// Standard
#define AUDIO_CLASS			0x01	// Audio
#define COM_CLASS			0x02	// Communication
#define	HID_CLASS			0x03	// Human Interface Device
#define MONITOR_CLASS		0x04	// Monitor
#define PHYSICAL_CLASS		0x05	// Physical Interface
#define POWER_CLASS			0x06	// Power
#define PRINTER_CLASS		0x07	// Printer
#define	MSS_CLASS			0x08	// Mass Storage Subsystem
#define	HUB_CLASS			0x09	// Hub

/// <summary>
/// Common descriptor header
/// </summary>
typedef struct {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
} __attribute__ ((packed)) DescHeader;

/// <summary>
/// Device descriptor
/// </summary>
typedef struct {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned short	bcdUSB;
	unsigned char	bDeviceClass;
	unsigned char	bDeviceSubClass;
	unsigned char	bDeviceProtocol;
	unsigned char	bMaxPacketSize0;
	unsigned short	idVendor;
	unsigned short	idProduct;
	unsigned short	bcdDevice;
	unsigned char	iManufacturer;
	unsigned char	iProduct;
	unsigned char	iSerialNumber;
	unsigned char	bNumConfigurations;
} __attribute__ ((packed)) DevDesc;

/// <summary>
/// Endpoint descriptor
/// </summary>
typedef struct {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned char	bEndpointAddress;
	unsigned char	bmAttributes;
	unsigned short	wMaxPacketSize;
	unsigned char	bInterval;
} __attribute__ ((packed)) EPDesc;

/// <summary>
/// Interface descriptor
/// </summary>
typedef struct {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned char	bInterfaceNumber;
	unsigned char	bAlternateSetting;
	unsigned char	bNumEndpoints;
	unsigned char	bInterfaceClass;
	unsigned char	bInterfaceSubClass;
	unsigned char	bInterfaceProtocol;
	unsigned char	iInterface;
} __attribute__ ((packed)) IntfDesc;

/// <summary>
/// Configuration descriptor
/// </summary>
typedef struct {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned short	wTotalLength;
	unsigned char	bNumInterfaces;
	unsigned char	bConfigurationValue;
	unsigned char	iConfiguration;
	unsigned char	bmAttributes;
	unsigned char	MaxPower;
} __attribute__ ((packed)) CfgDesc;

/// <summary>
/// String descriptor
/// </summary>
typedef struct {
	unsigned char	bLength;
	unsigned char	bDescriptorType;
	unsigned short	wLANGID[1];
} __attribute__ ((packed)) StrDesc;
#define	bString		wLANGID

/// <summary>
/// Language ID
/// </summary>
#define LANGID_ENG_US	0x0409


//#include "usbhid.h"
//#include "usbhub.h"

/// <summary>
/// USB Device
/// </summary>
//#ifdef STRUCT_SL811USBDEVINFO
typedef struct st_usbdevice
{
	SL811USBDevInfo devinfo;
	
	int address;
	int speed;
	
	DevDesc devdesc;
	CfgDesc *cfgdesc;

	IntfDesc	*intfdesc;
	HIDDesc		*hiddesc;
	RptDesc		*repdesc;
	EPDesc		*epdesc;
	HubDesc		*hubdesc;
	
	int			child_num;
	struct st_usbdevice	**child;
	struct st_usbdevice	*parent;
} USBDevice;
//#define	STRUCT_USB_DEVICE

typedef struct usb_int_queue_item
{
	int	interval;
	int	cnt;
	USBDevice	*device;
	int ep;
	void (*handler)(USBDevice*);
	struct usb_int_queue_item	*next;
} USBIntQueueItem;

//#endif

#define MAX_DEVICE_NUM	128

#endif
