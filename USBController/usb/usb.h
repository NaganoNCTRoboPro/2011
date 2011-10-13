#ifndef USB_H
#define USB_H

#include <sl811.h>
#include <wait.h>
#include<avr/pgmspace.h>
#define NULL (void*)0

/**
 * HID Class Specific Requests
 */
#define	HID_TYPE			0x21
#define	REPORT_TYPE			0x22
#define	PHYSICAL_TYPE		0x23

/**
 * HID Protocol
 */
#define	BOOT_PROTOCOL		0
#define	REPORT_PROTOCOL		1

#define	RPT_TYPE_INPUT		0x01
#define	RPT_TYPE_OUTPUT		0x02
#define	RPT_TYPE_FEATURE	0x03

/**
 * HID bRequest Value
 */
#define	GET_REPORT			 0x01
#define	GET_IDLE			 0x02
#define	GET_PROTOCOL		 0x03
#define	SET_REPORT			 0x09
#define	SET_IDLE			 0x0a
#define	SET_PROTOCOL		 0x0b

/**
 * HID Descriptor
 */
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


/**
 * Report Descriptor
 */
typedef struct {
	unsigned char bReport;
} __attribute__ ((packed)) RptDesc;

/**
 * Physical Descriptor
 */
typedef struct {
	unsigned char bPhysical;
} __attribute__ ((packed))  HidPhyDesc;

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

typedef struct {
	unsigned char	bDescLength;
	unsigned char	bDescriptorType;
	unsigned char	bNbrPorts;
	unsigned short	wHubCharacteristics;
	unsigned char	bPwrOn2PwrGood;
	unsigned char	bHubContrCurrent;
	unsigned char	DeviceRemovable[1];
	unsigned char	PortPwrCtrlMask[1];
} __attribute__ ((packed)) HubDesc;

/// <summary>
/// USB Device Speed Status
/// </summary>
#define USB_LOW_VIA_HUB		3

/// <summary>
/// Hub bRequest Value
/// </summary>
#define GET_STATE			0x02

/// <summary>
/// Type of Hub Descriptor
/// </summary>
#define	HUB_TYPE			0x29


/// <summary>
/// Hub Class Feature Selectors
/// </summary>
#define	C_HUB_LOCAL_POWER		0x00	// Recipient: Hub
#define	C_HUB_OVER_CURRENT		0x01	// Recipient: Hub

#define	PORT_CONNECTION			0x00	// Recipient: Port
#define	PORT_ENABLE				0x01	// Recipient: Port
#define	PORT_SUSPEND			0x02	// Recipient: Port
#define	PORT_OVER_CURRENT		0x03	// Recipient: Port
#define	PORT_RESET				0x04	// Recipient: Port
#define	PORT_POWER				0x08	// Recipient: Port
#define	PORT_LOW_SPEED			0x09	// Recipient: Port
#define	C_PORT_CONNECTION		0x10	// Recipient: Port
#define	C_PORT_ENABLE			0x11	// Recipient: Port
#define	C_PORT_SUSPEND			0x12	// Recipient: Port
#define	C_PORT_OVER_CURRENT		0x13	// Recipient: Port
#define	C_PORT_RESET			0x14	// Recipient: Port

/// <summary>
/// Port Bus State
/// </summary>
typedef unsigned char BusState;

/// <summary>
/// Bus State Bit Masks
/// </summary>
#define	BUS_STATE_D_MINUS_SIGNAL	0x01
#define	BUS_STATE_D_PLUS_SIGNAL		0x02

/// <summary>
/// Hub Status & Hub Change
/// </summary>
typedef struct{
	unsigned short	wHubStatus;
	unsigned short	wHubChange;
} __attribute__ ((packed)) HubStatCng;

/// <summary>
/// wHubStatus Bit Masks
/// </summary>
#define	MASK_HUB_LOCAL_POWER		0x0001
#define	MASK_HUB_OVER_CURRENT		0x0002

/// <summary>
/// wHubChange Bit Masks
/// </summary>
#define	MASK_C_HUB_LOCAL_POWER		0x0001
#define	MASK_C_HUB_OVER_CURRENT		0x0002

/// <summary>
/// Port Status & Port Change
/// </summary>
typedef struct{
	unsigned short	wPortStatus;
	unsigned short	wPortChange;
} __attribute__ ((packed)) PortStatCng;

/// <summary>
/// wPortStatus Bit Masks
/// </summary>
#define	MASK_PORT_CONNECTION		0x0001
#define	MASK_PORT_ENABLE			0x0002
#define MASK_PORT_SUSPEND			0x0004
#define MASK_PORT_OVER_CURRENT		0x0008
#define MASK_PORT_RESET				0x0010
#define MASK_PORT_POWER				0x0100
#define MASK_PORT_LOW_SPEED			0x0200

/// <summary>
/// wPortChange Bit Masks
/// </summary>
#define	MASK_C_PORT_CONNECTION		0x0001
#define	MASK_C_PORT_ENABLE			0x0002
#define	MASK_C_PORT_SUSPEND			0x0004
#define	MASK_C_PORT_OVER_CURRENT	0x0008
#define	MASK_C_PORT_RESET			0x0010

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

int USBInit(void);
void USBIntTxManagerInit(void);
void SL811DeviceInit(SL811USBDevInfo *d,SWord ep, SWord epsize, SWord retry, SWord pre, SWord addr);
SWord USBDeviceInit_in3(USBDevice *d, USBDevice *parent, SWord speed);
SWord Get_Descriptor(USBDevice *d, Byte type, void *data, SWord length, Byte index, SWord lang);
SWord CtrlTransfer(USBDevice *d, CtrlReq *command, Byte *data);
SWord Set_Address(USBDevice *d, SWord addr);
SWord Set_Configuration(USBDevice *d, SWord config);
SWord Get_Configuration(USBDevice *d, Byte *config);
SWord Sony_SIXAXIS_Special_Request(USBDevice *d);
SWord Get_Report(USBDevice *d, SWord type, Byte *data, size_t length, Byte report_id, SWord intf);
SWord USBAddIntTxTask(USBDevice *d, SWord ep, SWord interval, void (*fp)(USBDevice*));
SWord IntInTransfer(USBDevice *d, SWord ep, Byte *data, SWord size);
SWord SL811IntInTransfer(SL811USBDevInfo *d, Byte *data, SWord size);
SWord SL811CtrlTransfer(SL811USBDevInfo *d, CtrlReq *command, Byte *data);

#endif
