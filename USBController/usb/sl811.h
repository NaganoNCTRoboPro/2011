#ifndef SL811_H
#define SL811_H

#include <avr/io.h>
#include <wait.h>

#define NULL (void*)0
#define _DELAY_US(t) wait_us(t)
#define _DELAY_MS(t) wait_ms(t)

typedef uint8_t Byte;
typedef int16_t SWord;

typedef struct {
	Byte	addr;		// USBデバイスアドレス
	Byte	ep;			// エンドポイント番号
	Byte	epsize;		// エンドポイントのサイズ
	Byte	retry;		// リトライ回数
	Byte	wr_cmd;		// 送信用パケットのトグル
	Byte	rd_cmd;		// 受信用パケットのトグル
	Byte	pre;		// PREパケットの有無
} SL811USBDevInfo;

#define SL811WRITE_OUT		0
#define SL811WRITE_SETUP	1

#define SL811_USB_NONE		0
#define SL811_USB_LOW		1
#define SL811_USB_FULL		2

#define SL811_HANDSHAKE_NAK		(-1)
#define SL811_HANDSHAKE_STALL	(-2)

#define SL811H_HOSTCTLREG_A		0x00	// USB-A Host Control Register
#define SL811H_HOSTBASEADDR_A	0x01	// USB-A Host Base Address
#define SL811H_HOSTBASELNG_A	0x02	// USB-A Host Base Length
#define SL811H_PIDDEVENDPT_A	0x03	// USB-A Host PID, Device Endpoint(Write)
#define SL811H_USBSTATUS_A		0x03	// USB-A USB Status (Read)
#define SL811H_HOSTDEVADDR_A	0x04	// USB-A Host Device Address (Write)
#define SL811H_TRANSFERCNT_A	0x04	// USB-A Transfer Count (Read)
#define SL811H_CTLREG1			0x05	// Control Register 1
#define SL811H_INTENBLREG		0x06	// Interrupt Enable Register
#define SL811H_HOSTCTLREG_B		0x08	// USB-B Host Control Register
#define SL811H_HOSTBASEADDR_B	0x09	// USB-B Host Base Address
#define SL811H_HOSTBASELNG_B	0x0a	// USB-B Host Base Length
#define SL811H_PIDDEVENDPT_B	0x0b	// USB-B Host PID, Device Endpoint (Write)
#define SL811H_USBSTATUS_B		0x0b	// USB-B USB Status (Read)
#define SL811H_HOSTDEVADDR_B	0x0c	// USB-B Host Device Address (Write)
#define SL811H_TRANSFERCNT		0x0c	// USB-B Transfer Count (Read)
#define SL811H_INTSTATUSREG		0x0d	// Interrupt Status Register
#define SL811H_SOFCNTLOW		0x0e	// SOF Counter LOW (Write)
#define SL811H_HWREVREG			0x0e	// HW Revision Register (Read)
#define SL811H_SOFCNTHIGH		0x0f	// SOF Counter HIGH
#define SL811H_CTLREG2			0x0f	// Control Register 2

#define SL811H_MEMBUF_BGN		0x10	// Memory Buffer Begin
#define SL811H_MEMBUF_END		0xff	// Memory Buffer End

// USB-A/USB-B Host Control Register Definition [Address 0x00h, 0x08]
#define DATA0_WR    0x07	// (Arm+Enable+tranmist to Host+DATA0)
#define DATA1_WR    0x47	// (Arm+Enable+tranmist to Host on DATA1)
#define ZDATA0_WR   0x05	// (Arm+Transaction Ignored+tranmist to Host+DATA0)
#define ZDATA1_WR   0x45	// (Arm+Transaction Ignored+tranmist to Host+DATA1)
#define DATA0_RD    0x03	// (Arm+Enable+received from Host+DATA0)
#define DATA1_RD    0x43	// (Arm+Enable+received from Host+DATA1)

#define TOGGLE		0x40	// (Data Toggle Bit)
#define SYNC_SOF	0x20	// (SyncSOF)
#define PREAMBLE	0x80	// (Preamble)

// Host PID and Device Endpoint Register - Bits [Address 0x03, 0x0B]
#define PID_SETUP	0xD0
#define PID_IN		0x90
#define PID_OUT		0x10
#define PID_SOF		0x50
#define PID_PRE		0xc0
#define PID_NAK		0xa0
#define PID_STALL	0xe0
#define PID_DATA0	0x30
#define PID_DATA1	0xb0

// Packet Status Register - Bits [Address 0x03, 0x0B]
#define SL811H_STATUSMASK_ACK	0x01
#define SL811H_STATUSMASK_ERROR	0x02
#define SL811H_STATUSMASK_TMOUT	0x04
#define SL811H_STATUSMASK_SEQ	0x08
#define SL811H_STATUSMASK_SETUP	0x10
#define SL811H_STATUSMASK_OVF	0x20
#define SL811H_STATUSMASK_NAK	0x40
#define SL811H_STATUSMASK_STALL	0x80


// Control Register 1 - Bits [Address 0x05]
#define SL811H_CTLREG1MASK_ENABLESOF	0x01
#define SL811H_CTLREG1MASK_NOTXEOF2		0x04
#define SL811H_CTLREG1MASK_DSTATE		0x18
#define SL811H_CTLREG1MASK_LOWSPEED		0x20
#define SL811H_CTLREG1MASK_FULLSPEED	0x00
#define SL811H_CTLREG1MASK_SUSPEND		0x40
#define SL811H_CTLREG1MASK_CLK12MHZ		0x80

#define SL811H_CTLREG1VAL_RESET			0x08


// Interrupt Enable [Address 0x06] and Interrupt Status Register Bits [Address 0x0D]
#define SL811H_INTMASK_XFERDONE_A	0x01
#define SL811H_INTMASK_XFERDONE_B	0x02
#define SL811H_INTMASK_SOFINTR		0x10
#define SL811H_INTMASK_INSRMV		0x20
#define SL811H_INTMASK_DEVRESUME	0x40
#define SL811H_INTMASK_NODEVICE		0x40
#define SL811H_INTMASK_DSTATE		0x80	// only in status reg
#define SL811H_INTMASK_DATAPLUS		0x80


// SOF Counter HIGH and Control Register 2 [Address 0x0F]
#define SL811H_CTLREG2MASK_SOFHIGHCNTREG	0x3f	// SOF HIGH Counter Register
#define SL811H_CTLREG2MASK_DSWAP			0x40	// SL811 D+/D- Data Polarity Swap
#define SL811H_CTLREG2MASK_HOSTMODE			0x80	// SL811 Master/Slave selection

extern void sl811_init(void);
extern Byte sl811_read(const Byte offset);
extern void sl811_write(const Byte offset, const Byte data);
extern void sl811_buf_read(const Byte offset, Byte *buf, SWord size);
extern void sl811_buf_write(const Byte offset, const Byte *buf, SWord size);
extern SWord SL811WriteData(SL811USBDevInfo *d, Byte *data, SWord size, SWord flag);
extern SWord SL811ReadData(SL811USBDevInfo *d, Byte *data, SWord size);
extern int SL811USBReset(void);
extern int SL811USBInit(void);
extern int sl811_regtest(void);
extern void SL811SetAddress(SL811USBDevInfo *d, SWord addr);

#endif
