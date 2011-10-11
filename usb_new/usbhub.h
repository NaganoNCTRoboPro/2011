#ifndef __USB_HUB_H__
#define __USB_HUB_H__

#include"sl811.h"
#include"usb.h"
#include"sl811hcd.h"
#include"usbhid.h"
#include"other.h"
#include"sony_sixaxis.h"

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


#endif
