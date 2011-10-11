#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<avr/pgmspace.h>
#include<uart.h>

#include"printf.h"
#include"uart.h"
#include"macro.h"
#include"sl811.h"
#include"usb.h"
#include"sl811hcd.h"
#include"usbhid.h"
#include"usbhub.h"
#include"sony_sixaxis.h"
#include"rc_sixaxis.h"
#include"mu2.h"
//#include"mu2_common.h"
//#include"mu2_control.h"
//#include"mu2_data.h"

//WAIT_TIME_uS/mS
// _delay_ms(TIME)
// _delay_us(TIME)
// DELAY -->100uS
// DELAY2-->500mS

void initialize(void);
void mem_init(void);

void sl811_init(void);
unsigned char sl811_read(const unsigned char offset);
void sl811_write(const unsigned char offset, const unsigned char data);
void sl811_buf_read(const unsigned char offset, unsigned char *buf, int size);
void sl811_buf_write(const unsigned char offset, const unsigned char *buf, int size);
int SL811WriteData(SL811USBDevInfo *d, unsigned char *data, int size, int flag);
int SL811ReadData(SL811USBDevInfo *d, unsigned char *data, int size);

int USBInit(void);
void USBIntTxManagerInit(void);
int SL811USBInit(void);
int sl811_regtest(void);
int SL811USBReset(void);
int USBDeviceInit_in3(USBDevice *d, USBDevice *parent, int speed);
void SL811DeviceInit(SL811USBDevInfo *d, int ep, int epsize, int retry, int pre, int addr);
int Get_Descriptor(USBDevice *d, byte type, void *data, int length, byte index, word lang);
int CtrlTransfer(USBDevice *d, CtrlReq *command, byte *data);
int SL811CtrlTransfer(SL811USBDevInfo *d, CtrlReq *command, unsigned char *data);
int Set_Address(USBDevice *d, int addr);
void SL811SetAddress(SL811USBDevInfo *d, int addr);
int Set_Configuration(USBDevice *d, int config);
int Get_Configuration(USBDevice *d, byte *config);
/*
void print_desc(void *desc, int type);
static void print_devdesc(DevDesc *desc);
static void print_cfgdesc(CfgDesc *desc);
static void print_strdesc(StrDesc *desc);
static void print_intfdesc(IntfDesc *desc);
static void print_epdesc(EPDesc *desc);
static void print_hubdesc(HubDesc *desc);
static void print_hiddesc(HIDDesc *desc);
*/
int Sony_SIXAXIS_Init(USBDevice *d);
static int Sony_SIXAXIS_Special_Request(USBDevice *d);
int Get_Report(USBDevice *d, word type, byte *data, size_t length, byte report_id, word intf);
int USBAddIntTxTask(USBDevice *d, int ep, int interval, void (*fp)(USBDevice*));
static void Sony_SIXAXIS_IntTask(USBDevice *d);
int IntInTransfer(USBDevice *d, int ep, byte *data, int size);
int SL811IntInTransfer(SL811USBDevInfo *d, unsigned char *data, int size);

void controler_main_process_loop(void);
unsigned char *Toggle_SIXAXIS_Buffer(void);

union controller_data *Toggle_RC_Tx_Buffer(void);

volatile unsigned char DBGCHAR = 0;
volatile unsigned char *__sl811_addr, *__sl811_data;
static volatile unsigned int ms_tmr_cnt;
static int __usb_dev_mode;
static byte __device_addr[MAX_DEVICE_NUM];
static USBIntQueueItem *usbqueue;
static volatile int usb_int_tx_enable;
static unsigned char _sixaxis_buf[SIXAXIS_DATA_LENGTH][2];
static unsigned char _sixaxis_buf_index;
static unsigned char _sixaxis_report_buf[SIXAXIS_REPORT_LENGTH];
static union controller_data _rc_tx_buf[2];
static unsigned char _rc_tx_buf_selector;
static int _rc_tx_buf_index;
USBDevice dev;
CfgDesc	config;
SIXAXIS *sixaxis;
//static RptDesc reprt;
static USBIntQueueItem _newitem;
union controller_data *txdata;

unsigned char TxDataBuf[RC_DATA_LENGTH*4];				//uart�̑��M�p�o�b�t�@�����Ɉꎞ�I�ɑ��M����f�[�^��~����
unsigned char Group[4] 		 = {0x21,0x9A,0x44,0x80};	//�O���[�v���X�g
unsigned char Channel[4][4]  ={{0x07,0x08,0x09,0x0A},	//Channel List
						 	   {0x0C,0x0D,0x0E,0x0F},
							   {0x25,0x26,0x27,0x28},
							   {0x2A,0x2B,0x2C,0x2D}};

	
int main(void)
{
	int i, k = 0;
	int RESETCounter = 0;
	volatile int tmp, speed;

	initialize();
	mem_init();
//	uart_init(0,UART_TE|UART_RE,9600);
	uart_init(0,UART_TE,19200);
//	uart_init(1,UART_TE,19200);

	set_printf_output(uart0_putchar);
/*
	_delay_ms(100);
	_delay_ms(100);
	_delay_ms(100);
	uart0_putchar('@');
	uart0_putchar('B');
	uart0_putchar('R');
	uart0_putchar('4');
	uart0_putchar('8');
	uart0_putchar(0x0d);
	uart0_putchar(0x0a);
	_delay_ms(100);
	_delay_ms(100);
	_delay_ms(100);
*/
RESET:

	LED_OFF_ALL;
	
	cli();	

	LED0_ON;

//**/	DBG("USB_SET_Stert\n");

	VBUS_ON;
	for(i = 0;i <= 20; i++ ) _delay_ms(250);

	tmp = USBInit();
	if( tmp == 0 )
	{
		speed = SL811USBReset();
//**/		DBG("USBReset() - %s\n", ((speed == USB_NONE) ? "NONE" : ((speed == USB_LOW) ? "LOW" : ((speed == USB_FULL) ? "FULL" : "???"))));
		if( speed == USB_LOW || speed == USB_FULL )
		{
//**/			DBG("Device initialize...\n");
			LED_OFF_ALL;
			LED1_ON;
			for( i=0; i<5; i++ )
			{
				tmp = USBDeviceInit_in3(&dev, NULL, speed);
//**/				DBG("return_tmp[USBDeviceInit_in3]-->%d\n",tmp);
				if( tmp == 0 )	break;
				_delay_ms(100);
			}
			if( tmp == 0 )
			{
//**/				DBG("Device initialize succeed!\n");
//**/				DBG("Sony SIXAXIS\n");
				LED_OFF_ALL;
				LED0_ON;
				LED1_ON;
				_delay_ms(100);
				while(1)
				{
					tmp = Sony_SIXAXIS_Init(&dev);
					_delay_ms(250);	// �m����SIXAXIS����f�[�^����M
//**/					DBG("return_tmp[Sony_SIXAXIS_Init]-->%d\n",tmp);
					if(tmp==0)break;
					if(k>5)goto ERROR;
					k++;
				}
				LED_OFF_ALL;
				LED2_ON;
				controler_main_process_loop();
			}
			else
			{
//**/				DBG("Device initialize error!\n");
				if(k>10)goto ERROR;
				k++;
				goto RESET;			
				//while(1);
			}
		}
	}
	else
	{
//**/		DBG("Initialize Error...\n");
		goto RESET;
		//while(1);
	}
	
	if(RESETCounter<1)
	{
		RESETCounter++;
		goto RESET;
	}
ERROR:
	k = 0;
	while(1)
	{
		LED0_ON;
		LED2_ON;
		for(i = 0;i<10; i++)
		{
			_delay_ms(250);
		}
		LED_OFF_ALL;
		for(i = 0;i<10; i++)
		{
			_delay_ms(250);
		}
		if(k>1)
		{	
			i = 0;
			k = 0;
			RESETCounter = 0;
			goto RESET;
		}
		k++;
	}

	while(1);

	return 0;
}

//�ȉ��֐�

void initialize(void)//IO_SETING
{
	DDRD = 0xFE;//PD1~7-->OUT
	PORTD = 0x82;//PD 1000 001*
	DDRE = 0x02;
	DDRF = 0x00;
	PORTF = 0xFF;
}

void mem_init(void)//XMEM_SETING
{
	MCUCR |= _BV(SRE) | _BV(SRW10);//XMEM_PARMISSION
	XMCRA |= _BV(SRW11) | _BV(SRW00) | _BV(SRW01);//(High&Low)WAIT2+LOCK1
	XMCRB |= _BV(XMBK);//BUS_KEEPE
}

/// <summary>
/// SL811HST�̏�����
/// </summary>
void sl811_init(void)
{
	__sl811_addr = (volatile unsigned char *)0x8000;
	__sl811_data = (volatile unsigned char *)0xC000;
}

/// <summary>
/// SL811HST��p���W�X�^����̓ǂݍ���
/// </summary>
unsigned char sl811_read(const unsigned char offset)
{
	unsigned char data;

	*(__sl811_addr) = offset;
	DELAY;
	data = *(__sl811_data);
	DELAY;

	return data;
}

/// <summary>
/// SL811HST��p���W�X�^�ւ̏�������
/// </summary>
void sl811_write(const unsigned char offset, const unsigned char data)
{
	*(__sl811_addr) = offset;
	DELAY;
	*(__sl811_data) = data;
	DELAY;
}

/// <summary>
/// SL811HST FIFO�������E�o�b�t�@����̃u���b�N�E�f�[�^�ǂݍ���
/// </summary>
void sl811_buf_read(const unsigned char offset, unsigned char *buf, int size)
{
	if(size <= 0)	return;
	
	*(__sl811_addr) = offset;
	DELAY;

	while( size-- ){
		 *buf = *(__sl811_data);
		 DELAY;
		 buf++;
	}
}

/// <summary>
/// SL811HST FIFO�������E�o�b�t�@�փu���b�N�E�f�[�^��������
/// </summary>
void sl811_buf_write(const unsigned char offset, const unsigned char *buf, int size)
{
	if(size <= 0)	return;
	
	*(__sl811_addr) = offset;
	DELAY;

	while( size-- ){
		*(__sl811_data) = *buf;
		DELAY;
		buf++;
	}
}

/// <summary>
/// �p�P�b�g�̑��M (USB-A�𗘗p)
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="data">���M�f�[�^</param>
/// <param name="size">���M�f�[�^�̃T�C�Y</param>
/// <param name="flag">
/// OUT�p�P�b�g�ESETUP�p�P�b�g�̑I��
/// SL811WRITE_OUT [0] �̏ꍇ��OUT�p�P�b�g
/// SL811WRITE_SETUP [1] �̏ꍇ��SETUP�p�P�b�g
/// </param>
/// <returns>
/// ����I���� : ���M�����f�[�^�̃T�C�Y
/// �s���I���� : SL811_HANDSHAKE_NAK [-1] �������� SL811_HANDSHAKE_STALL [-2]
/// </returns>
int SL811WriteData(SL811USBDevInfo *d, unsigned char *data, int size, int flag)
{
	static volatile unsigned char pktstatus;
	int remain, wsiz, timeovr, retry;
	
	// �������̃f�[�^��
	remain = size;

	retry = d->retry;
	d->wr_cmd |= d->pre;
	
	// USB FIFO�������o�b�t�@�̐擪�A�h���X�̎w��
	sl811_write(SL811H_HOSTBASEADDR_A, SL811H_MEMBUF_BGN);

	// �ΏۂƂȂ�USB�f�o�C�X�̃A�h���X�̎w��
	sl811_write(SL811H_HOSTDEVADDR_A, d->addr);

	// �p�P�b�g���(OUT/SETUP)�G���h�|�C���g�ԍ��̎w��
	sl811_write(SL811H_PIDDEVENDPT_A, (flag == SL811WRITE_SETUP) ? PID_SETUP : (PID_OUT | d->ep));

	do{
		// 1�̃p�P�b�g�ő��M����f�[�^�����Z�o
		wsiz = (remain > d->epsize) ? d->epsize : remain;

		// ���M�f�[�^������ꍇ�́AUSB FIFO�������o�b�t�@�֓]������
		if( data != NULL && size > 0 )
			sl811_buf_write(SL811H_MEMBUF_BGN, &data[size - remain], wsiz);
		
		// ���M�f�[�^�̑傫�����Z�b�g
		sl811_write(SL811H_HOSTBASELNG_A, wsiz);

		// USB�f�o�C�X��NAK�̏ꍇ�A�ēxUSB�f�[�^���M������
		for( timeovr=0; timeovr<retry; timeovr++ )
		{

			// USB_A����USB FIFO�������o�b�t�@�̓��e�𑗐M����
			sl811_write(SL811H_HOSTCTLREG_A, d->wr_cmd);

			_delay_ms(200);

			// USB_A�̑��M����������܂ő҂�
			while( (sl811_read(SL811H_INTSTATUSREG) & SL811H_INTMASK_XFERDONE_A) == 0 );

			// USB_A�̃p�P�b�g���M��Ԃ�ǂݍ���
			pktstatus = sl811_read(SL811H_USBSTATUS_A);

			// USB�f�o�C�X��STALL�̏ꍇ�̓G���[�I��
			if( pktstatus & SL811H_STATUSMASK_STALL )
			{
//**/				DBG("!!! READ STALL !!!\n");

				return SL811_HANDSHAKE_STALL;
			}
			// USB�f�o�C�X����ACK���Ԃ��Ă���΁A���[�v�𔲂���
			if( pktstatus & SL811H_STATUSMASK_ACK )	break;

			// ���g���C�̏ꍇ��5ms�҂�
			_delay_ms(5);			
		}

		// USB�f�[�^���M�̌��ʂ����g���C�����Ă�ACK�łȂ��ꍇ�́A�G���[�I��
		if( !(pktstatus & SL811H_STATUSMASK_ACK) )
		{
//**/			DBG("!!! WRITE TIMEOVER !!!\n");

			return SL811_HANDSHAKE_NAK;
		}
		
		// �����ς݂̃f�[�^������������
		remain -= wsiz;

		// �f�[�^�p�P�b�g�̃g�O�����s��
		d->wr_cmd ^= TOGGLE;
	}while( remain>0 );	// �c��f�[�^������ԁA�J��Ԃ�

	return size;	// USB�f�[�^���M�����T�C�Y��Ԃ�
}

/// <summary>
/// �p�P�b�g�̎�M (USB-B�𗘗p)
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="data">��M�f�[�^</param>
/// <param name="size">��M�f�[�^�̃T�C�Y</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : SL811_HANDSHAKE_NAK [-1] �������� SL811_HANDSHAKE_STALL [-2]
/// </returns>
int SL811ReadData(SL811USBDevInfo *d, unsigned char *data, int size)
{
	unsigned char pktstatus;
	int	timeovr, remain, rsiz, rcnt;
	int	maxretry, minretry;
	int	fifo;

	// USB FIFO�������o�b�t�@�̐擪�A�h���X�̏����l�ݒ�
	fifo = SL811H_MEMBUF_BGN;
	
	// �������̃f�[�^��
	remain = size;

	// USB�f�o�C�X��NAK�̂Ƃ��̃��g���C��
	maxretry = d->retry;

	// �Z���҂����Ԃł̃��g���C��
	minretry = (maxretry <= 7) ? maxretry : maxretry / 3;

	d->rd_cmd |= d->pre;

	// �ΏۂƂȂ�USB�f�o�C�X�A�h���X�̎w��
	sl811_write(SL811H_HOSTDEVADDR_B, d->addr);

	// �p�P�b�g���(IN)�ƃG���h�|�C���g�ԍ��̎w��
	sl811_write(SL811H_PIDDEVENDPT_B, PID_IN | d->ep);

	do{
		// 1�̃p�P�b�g�ő��M����f�[�^�����Z�o
		rsiz = (remain > d->epsize) ? d->epsize : remain;

		// ���݂�USB FIFO�������o�b�t�@��SL811H_MEMBUF_END(FFH)�𒴂���ꍇ�A�����l�ݒ�
		if( (fifo + rsiz) > SL811H_MEMBUF_END )
			fifo = SL811H_MEMBUF_BGN;	

		// ���݂�USB FIFO�������o�b�t�@�̐擪�A�h���X�̎w��
		sl811_write(SL811H_HOSTBASEADDR_B, fifo);
		
		// ��M�f�[�^�̑傫�����Z�b�g
		sl811_write(SL811H_HOSTBASELNG_B, rsiz);

		// USB�f�o�C�X��NAK�̏ꍇ�A�ēxUSB�f�[�^��M������
		for( timeovr=0; timeovr<maxretry; timeovr++ )
		{

			// USB_B���USB FIFO�������o�b�t�@�֓��e����M����
			sl811_write(SL811H_HOSTCTLREG_B, d->rd_cmd);

			// 200us �܂��� 2ms �̎��ԑ҂�
			_delay_ms(2);

			// USB_B�̎�M����������܂ő҂�
			while( (sl811_read(SL811H_INTSTATUSREG) & SL811H_INTMASK_XFERDONE_B) == 0 );
			
			// USB_B�̃p�P�b�g��M��Ԃ�ǂݍ���
			pktstatus = sl811_read(SL811H_USBSTATUS_B);

			// USB�f�o�C�X��STALL�̏ꍇ�̓G���[�I��
			if( pktstatus & SL811H_STATUSMASK_STALL )
			{
//**/				DBG("!!! READ STALL !!!\n");

				return SL811_HANDSHAKE_STALL;
			}
			// USB�f�o�C�X����ACK���Ԃ��Ă���΁A���[�v�𔲂���
			if( pktstatus & SL811H_STATUSMASK_ACK )	break;
		}

		// USB�f�[�^��M�̌��ʂ����g���C�����Ă�ACK�łȂ��ꍇ�́A�G���[�I��
		if( timeovr >= maxretry )
		{
//**/			DBG("!!! READ TIMEOVER !!!\n");

			return SL811_HANDSHAKE_NAK;
		}

		// ���ۂɎ�M�������f�[�^�����Z�o
		rcnt = rsiz - (int)sl811_read(SL811H_TRANSFERCNT);

		// ��M�f�[�^������ꍇ�́AUSB FIFO�������o�b�t�@����]������
		if( (pktstatus & SL811H_STATUSMASK_ACK) && data != 0 && size > 0 )
			sl811_buf_read(fifo, &data[size - remain], rsiz);

		// �����ς݂̃f�[�^������������
		remain -= rcnt;

		// �f�[�^�p�P�b�g�̃g�O�����s��
		d->rd_cmd ^= TOGGLE;

		// ���݂�USB FIFO�������o�b�t�@�̐擪�A�h���X�̍X�V
		fifo += rcnt;
	}while( remain > 0 && rcnt == rsiz );	// �c��f�[�^������ԁA�J��Ԃ�

	return size - remain;	// USB�f�[�^��M�����T�C�Y��Ԃ�
}

/// <summary>
/// ���[�g�f�o�C�X�̃��Z�b�g
/// </summary>
/// <returns>
/// �f�o�C�X����: SL811_USB_NONE [0]
/// LowSpeed:     SL811_USB_LOW  [1]
/// FullSpeed:    SL811_USB_FULL [2]
/// </returns>
int SL811USBReset(void)
{
	int ret;
	unsigned char status;

//**/	DBG("Enter USBReset\n");
	_delay_ms(100);

	// Config host mode
	sl811_write(SL811H_CTLREG2, SL811H_CTLREG2MASK_HOSTMODE);

	sl811_write(SL811H_CTLREG1, SL811H_CTLREG1VAL_RESET);	// reset USB
	_delay_ms(30);
	sl811_write(SL811H_CTLREG1, 0x00);						// remove SE0

	for(status=0; status<100; status++)
		sl811_write(SL811H_INTSTATUSREG, 0xFF);				// clear all interrupt bits

	// Full/Low Speed Detection
	status = sl811_read(SL811H_INTSTATUSREG);
	
	if( status & SL811H_INTMASK_NODEVICE )
	{
		// USB device none
//**/        DBG("USBReset: Device Removed\n");

		sl811_write(SL811H_INTENBLREG,
			SL811H_INTMASK_SOFINTR 		// Interrupt on SOF Timer
		);
		return SL811_USB_NONE;
	}

	if( status & SL811H_INTMASK_DATAPLUS )
	{
		// Full speed device
		ret = SL811_USB_FULL;

//**/		DBG("USBReset: Full speed device attached\n");

		sl811_write(SL811H_SOFCNTLOW, 0xE0);	// ���� E0
		sl811_write(SL811H_SOFCNTHIGH, SL811H_CTLREG2MASK_HOSTMODE | 0x2E);		// SOF High 2E + HOSTMode
		sl811_write(SL811H_CTLREG1, SL811H_CTLREG1MASK_ENABLESOF | SL811H_CTLREG1MASK_FULLSPEED);

		// clear all interrupt bits
		sl811_write(SL811H_INTSTATUSREG, 0xFF);
	}
	else
	{
		// Low speed device
		ret = SL811_USB_LOW;

//**/		DBG("USBReset: Low speed device attached\n");

		sl811_write(SL811H_SOFCNTLOW, 0xE0);	// ���� E0
		sl811_write(SL811H_SOFCNTHIGH, SL811H_CTLREG2MASK_HOSTMODE | SL811H_CTLREG2MASK_DSWAP | 0x2E);	// SOF High 2E + HOSTMode + Data Priority Swap
		sl811_write(SL811H_CTLREG1, SL811H_CTLREG1MASK_ENABLESOF | SL811H_CTLREG1MASK_LOWSPEED);
		
		// clear all interrupt bits
		for(status=0; status<20; status++)
			sl811_write(SL811H_INTSTATUSREG, 0xFF);
	}

	// enable all interrupts
	sl811_write(SL811H_INTENBLREG,
		SL811H_INTMASK_SOFINTR		// Interrupt on SOF Timer
	);
	
//**/	DBG("INTENBLREG-->0x%02X\n",sl811_read(SL811H_INTENBLREG));			// Device Insert/Remove Detection
	
	return ret;
}

/// <summary>
/// SL811�̃��Z�b�g
/// </summary>
/// <returns>
/// ����I��: 0
/// �ُ�I��: -1
/// </returns>
int SL811USBInit(void)
{
	sl811_init();
	
	ms_tmr_cnt=0;
	
	return sl811_regtest();
}

/// <summary>
/// SL811HST FIFO�������E�o�b�t�@�̓���`�F�b�N�i���C�g�E���[�h�j
/// </summary>
/// <returns>
/// ����I��: 0
/// �ُ�I��: -1
/// </returns>
int sl811_regtest(void)
{
	int	i, data;

	for( i=SL811H_MEMBUF_BGN; i<SL811H_MEMBUF_END; i++ )
	{
		sl811_write(i, 256-i);
//**/		DBG("%x\n",i);
		data = sl811_read(i);
//**/		DBG("%x\n",data);
		if( data != (256-i) )
		{
//**/			DBG("ERRER-regtest-");
			return -1;
		}
	}
	/*for( i=SL811H_MEMBUF_BGN; i<SL811H_MEMBUF_END; i++ )
	{
		sl811_write(i, 256-i);
		DBG("%x\n",i);
	}
	
	for( i=SL811H_MEMBUF_BGN; i<SL811H_MEMBUF_END; i++ )
	{
		data = sl811_read(i);
		DBG("%x\n",data);
		if( data != (256-i) )
		{
			DBG("ERRER-regtest-");
			return -1;
		}
	}*/
	
	return 0;
}

/// <summary>
/// USB Initialize
/// </summary>
/// <returns>
/// ����I��: 0
/// �ُ�I��: -1
/// </returns>
int USBInit(void)
{
	int x;
	
	USBIntTxManagerInit();
	
	__usb_dev_mode = USB_NONE;
	
	x = SL811USBInit();
	
	return x;
}

/// <summary>
/// USB Interrupt Transfer Manager Initialize
/// </summary>
void USBIntTxManagerInit(void)
{
	usbqueue = NULL;
	usb_int_tx_enable = 1;
}

/// <summary>
/// USB�f�o�C�X�̏���������
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="parent">�e�f�o�C�X</param>
/// <param name="speed">�Ώۃf�o�C�X�̑��x</param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int USBDeviceInit_in3(USBDevice *d, USBDevice *parent, int speed)
{
	int devaddr;
	
//**/	int	i, epnum;
	byte tmp;
	
	// EndPoint No. - 0, EndPointSize - 8, Retry - 12, PRE - Preamble, Address - 0
	SL811DeviceInit(&(d->devinfo), 0, 8, 12 + ((parent != NULL && speed == USB_LOW) ? 8 : 0), ((parent != NULL && speed == USB_LOW) ? 1 : 0), 0);
	d->speed = speed;
	
	// �f�o�C�X�f�B�X�N�v���^�̉��擾
//**/	DBG("GET Device Descriptor\n");
	if( Get_Descriptor(d, DEVICE_TYPE, &(d->devdesc), 8, 0, 0) < 0 )	return -1;

	// �G���h�|�C���g�T�C�Y�̐ݒ�
	SL811DeviceInit(&(d->devinfo), 0, d->devdesc.bMaxPacketSize0, 20, 0, 0);
	
	// �f�o�C�X�A�h���X���Z�b�g
//**/	DBG("SET Device Address\n");
	devaddr = 1;
	if( Set_Address(d, devaddr) == -1 )	return -1;
	
	// �f�o�C�X�f�B�X�N�v���^�̎擾
//**/	DBG("GET Device Descriptor\n");
	if( Get_Descriptor(d, DEVICE_TYPE, &(d->devdesc), sizeof(DevDesc), 0, 0) < 0 )	return -1;

	// �R���t�B�O���[�V�����f�B�X�N�v���^�̉��擾
//**/	DBG("GET Configuration Descriptor\n");
	if( Get_Descriptor(d, CONFIG_TYPE, &config, sizeof(CfgDesc), 0, 0) < 0 )	return -1;

	// �R���t�B�O���[�V�����f�B�X�N�v���^�̎擾
//**/	DBG("GET Configuration Descriptor\n");
	d->cfgdesc = &config;
	if( Get_Descriptor(d, CONFIG_TYPE, d->cfgdesc, config.wTotalLength, 0, 0) < 0 )	return -1;
	
	// �|�C���^�̃Z�b�g
	d->intfdesc = (IntfDesc*)((byte*)d->cfgdesc + d->cfgdesc->bLength);

	if( d->devdesc.bDeviceClass == 0x00 &&
		d->intfdesc->bInterfaceClass == HID_CLASS ){
		
		d->hiddesc = (HIDDesc*)((byte*)d->intfdesc + d->intfdesc->bLength);
		d->epdesc = (EPDesc*)((byte*)d->hiddesc + d->hiddesc->bLength);

		// ���|�[�g�f�B�X�N���v�^�̎擾
//**/		DBG("GET Report Descriptor\n");
		if( d->hiddesc->Report[0].bDescriptorType == REPORT_TYPE )
		{
//			d->repdesc = &reprt;//&_repdesc;//(RptDesc*)malloc( (d->hiddesc->Report[0].wDescriptorLength) );
//			Get_Descriptor(d, REPORT_TYPE, d->repdesc, d->hiddesc->Report[0].wDescriptorLength, 0, 0);
//			DBG("GET Report Descriptor END\n");
		}
	}else{
		d->hiddesc = NULL;
		d->repdesc = NULL;
		d->epdesc = (EPDesc*)((byte*)d->intfdesc + d->intfdesc->bLength);
	}
	
	// �R���t�B�M�����[�V�����̐ݒ�
//**/	DBG("SET Configuration\n");
	Set_Configuration(d, d->cfgdesc->bConfigurationValue);
	
	// ���̕\��
	
	Get_Configuration(d, &tmp);
//**/	if( tmp != 0x00 ) 	DBG("Configuration Finished!\n");
/*
	DBG("          ---===>Device Information<===---\n");

	DBG("\nConnectionStatus:\n");
	DBG("  -> Device Bus Speed: ");
	DBG( (d->speed==USB_FULL) ? "Full\n" : "Low\n");
	DBG("Device Address:       0x%02X\n", d->address);

	print_desc(&d->devdesc, DEVICE_TYPE);
	print_desc(d->cfgdesc, CONFIG_TYPE);
	print_desc(d->intfdesc, INTERFACE_TYPE);
	
	if( d->devdesc.bDeviceClass == 0x00 &&
		d->intfdesc->bInterfaceClass == HID_CLASS ){
		print_desc(d->hiddesc, HID_TYPE);
	}

	epnum = d->intfdesc->bNumEndpoints;

	for( i=0; i<epnum; i++){
		print_desc(&d->epdesc[i], ENDPOINT_TYPE);
	}	
*/
	return 0;
}

/// <summary>
/// SL811 Device Initialize
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="ep">�G���h�|�C���g�ԍ�</param>
/// <param name="epsize">�G���h�|�C���g�̃T�C�Y</param>
/// <param name="retry">���g���C��</param>
/// <param name="pre">�v���A���u���p�P�b�g�̗L��</param>
/// <param name="addr">�f�o�C�X�̃A�h���X</param>
void SL811DeviceInit(SL811USBDevInfo *d, int ep, int epsize, int retry, int pre, int addr)
{
	d->ep = ep;
	d->epsize = epsize;
	d->retry = retry;
	d->pre = (pre) ? (PREAMBLE) : (0x00);
	d->addr = addr;
}

/// <summary>
/// Get Descriptor
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="type">�f�B�X�N���v�^�̎��</param>
/// <param name="data">��M����f�[�^</param>
/// <param name="length">��M����f�[�^�̃T�C�Y</param>
/// <param name="index">index�l USB�d�l�� Get Descriptor�Q��</param>
/// <param name="lang">lang�l USB�d�l�� Get Descriptor�Q��</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
int Get_Descriptor(USBDevice *d, byte type, void *data, int length, byte index, word lang)
{
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_HOST;
	
	if( type == HID_TYPE || type == REPORT_TYPE )	command.bmRequestType |= TRANS_TO_IF;

	command.bRequest = GET_DESCRIPTOR;
	command.wValue = (type << 8) | index;
	command.wIndex = lang;
	command.wLength = length;
	
	return CtrlTransfer(d, &command, (byte*)data);
}

/// <summary>
/// USB Control Transfer
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="command">���N�G�X�g</param>
/// <param name="data">���M�^��M����f�[�^</param>
/// <returns>
/// ����I���� : ���M�^��M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
int CtrlTransfer(USBDevice *d, CtrlReq *command, byte *data)
{
	d->devinfo.ep = 0;
	return SL811CtrlTransfer(&(d->devinfo), command, data);
}

/// <summary>
/// SL811 USB Control Transfer
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="command">���N�G�X�g</param>
/// <param name="data">���M�^��M����f�[�^</param>
/// <returns>
/// ����I���� : ���M�^��M�����f�[�^�̃T�C�Y
/// �s���I���� : SL811_HANDSHAKE_NAK [-1] �������� SL811_HANDSHAKE_STALL [-2]
/// </returns>
int SL811CtrlTransfer(SL811USBDevInfo *d, CtrlReq *command, unsigned char *data)
{
	int size, ret, length;

	d->wr_cmd = DATA0_WR;
	size = SL811WriteData(d, (unsigned char*)command, sizeof(CtrlReq), SL811WRITE_SETUP);	// SETUP

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

/// <summary>
/// Set Device Address
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="addr">�A�h���X</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
int Set_Address(USBDevice *d, int addr)
{
	CtrlReq command;
	int ret;

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

/// <summary>
/// SL811 Set Address
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="addr">�f�o�C�X�̃A�h���X</param>
void SL811SetAddress(SL811USBDevInfo *d, int addr)
{
	d->addr = addr;
}

/// <summary>
/// Set Configuration
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="config">�R���t�B�M�����[�V�����ԍ�</param>
/// <returns>
/// ����I���� : ���M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
int Set_Configuration(USBDevice *d, int config)
{
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_DEV;
	command.bRequest = SET_CONFIG;
	command.wValue = config;
	command.wIndex = 0;
	command.wLength = 0;

	return CtrlTransfer(d, &command, NULL);
}

/// <summary>
/// Get Configuration
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="config">�R���t�B�M�����[�V�����ԍ�</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
int Get_Configuration(USBDevice *d, byte *config)
{
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_HOST;
	command.bRequest = GET_CONFIG;
	command.wValue = 0;
	command.wIndex = 0;
	command.wLength = 1;

	return CtrlTransfer(d, &command, config);
}
/*
void print_desc(void *desc, int type)
{
	switch(type){
		case DEVICE_TYPE:		print_devdesc((DevDesc*)desc);		break;
		case CONFIG_TYPE:		print_cfgdesc((CfgDesc*)desc);		break;
		case STRING_TYPE:		print_strdesc((StrDesc*)desc);		break;
		case INTERFACE_TYPE:	print_intfdesc((IntfDesc*)desc);	break;
		case ENDPOINT_TYPE:		print_epdesc((EPDesc*)desc);		break;
			case HUB_TYPE:			print_hubdesc((HubDesc*)desc);		break;
			case HID_TYPE:			print_hiddesc((HIDDesc*)desc);		break;
			case REPORT_TYPE:											break;
			case PHYSICAL_TYPE:											break;
		default:													break;
	}
}

static void print_devdesc(DevDesc *desc)
{
	DBG("\n          ===>Device Descriptor<===\n");
	DBG("bLength:                           0x%02X\n", desc->bLength);
	DBG("bDescriptorType:                   0x%02X\n", desc->bDescriptorType);
	DBG("bcdUSB:                          0x%04X\n", desc->bcdUSB);
	DBG("bDeviceClass:                      0x%02X", desc->bDeviceClass);
	switch(desc->bDeviceClass){
		case HUB_CLASS:	DBG("  -> This is a HUB Device\n");	break;
		default:	DBG("\n");
	}
	DBG("bDeviceSubClass:                   0x%02X\n", desc->bDeviceSubClass);
	DBG("bDeviceProtocol:                   0x%02X\n", desc->bDeviceProtocol);
	DBG("bMaxPacketSize0:                   0x%02X = (%d) Bytes\n", desc->bMaxPacketSize0, desc->bMaxPacketSize0);
	DBG("idVendor:                        0x%04X\n", desc->idVendor);
	DBG("idProduct:                       0x%04X\n", desc->idProduct);
	DBG("bcdDevice:                       0x%04X\n", desc->bcdDevice);
	DBG("iManufacturer:                     0x%02X\n", desc->iManufacturer);
	DBG("iProduct:                          0x%02X\n", desc->iProduct);
	DBG("iSerialNumber:                     0x%02X\n", desc->iSerialNumber);
	DBG("bNumConfigurations:                0x%02X\n", desc->bNumConfigurations);
}

static void print_cfgdesc(CfgDesc *desc)
{
	DBG("\n          ===>Configuration Descriptor<===\n");
	DBG("bLength:                           0x%02X\n", desc->bLength);
	DBG("bDescriptorType:                   0x%02X\n", desc->bDescriptorType);
	DBG("wTotalLength:                    0x%04X\n", desc->wTotalLength);
	DBG("bNumInterfaces:                    0x%02X\n", desc->bNumInterfaces);
	DBG("bConfigurationValue:               0x%02X\n", desc->bConfigurationValue);
	DBG("iConfiguration:                    0x%02X\n", desc->iConfiguration);
	DBG("bmAttributes:                      0x%02X  -> ", desc->bmAttributes);
	DBG((desc->bmAttributes & 0x40) ? "Self Powered " : "Bus Powered ");
	DBG((desc->bmAttributes & 0x20) ? "Remote Wakeup" : "");
	DBG("\n");
	DBG("MaxPower:                          0x%02X = %d mA\n", desc->MaxPower, desc->MaxPower*2);
}

static void print_strdesc(StrDesc *desc)
{
	int i, str_len;

	str_len = (desc->bLength-2) / 2;

	putchar('"');
	for( i=0; i<str_len; i++ )
		putchar((char)(desc->bString[i] >> 8));
	putchar('"');
	putchar('\n');
}

static void print_intfdesc(IntfDesc *desc)
{
	DBG("\n          ===>Interface Descriptor<===\n");
	DBG("bLength:                           0x%02X\n", desc->bLength);
	DBG("bDescriptorType:                   0x%02X\n", desc->bDescriptorType);
	DBG("bInterfaceNumber:                  0x%02X\n", desc->bInterfaceNumber);
	DBG("bAlternateSetting:                 0x%02X\n", desc->bAlternateSetting);
	DBG("bNumEndpoints:                     0x%02X\n", desc->bNumEndpoints);
	DBG("bInterfaceClass:                   0x%02X", desc->bInterfaceClass);
	switch( desc->bInterfaceClass ){
		case HID_CLASS:	DBG("  -> HID Interface Class\n");	break;
		case HUB_CLASS:	DBG("  -> HUB Interface Class\n");	break;
		default:	DBG("\n");
	}
	DBG("bInterfaceSubClass:                0x%02X", desc->bInterfaceSubClass);
	switch( desc->bInterfaceSubClass ){
		case 1:		DBG("  -> Boot Interface\n");	break;
		default:	DBG("\n");
	}
	DBG("bInterfaceProtocol:                0x%02X", desc->bInterfaceProtocol);
	switch( desc->bInterfaceProtocol ){
		case 1:		DBG("  -> Keyboard\n");	break;
		case 2:		DBG("  -> Mouse\n");		break;
		default:	DBG("\n");
	}
	DBG("iInterface:                        0x%02X\n", desc->iInterface);
}

static void print_epdesc(EPDesc *desc)
{
	DBG("\n          ===>Endpoint Descriptor<===\n");
	DBG("bLength:                           0x%02X\n", desc->bLength);
	DBG("bDescriptorType:                   0x%02X\n", desc->bDescriptorType);
	DBG("bEndpointAddress:                  0x%02X  -> Direction: ", desc->bEndpointAddress);
	DBG((desc->bEndpointAddress & EPADDR_MASK_DIRECTION) ? "IN" : "OUT");
	DBG(" - EndpointID: %d\n", desc->bEndpointAddress & EPADDR_MASK_ADDRESS);
	DBG("bmAttributes:                      0x%02X  -> ", desc->bmAttributes);
	switch(desc->bmAttributes & 0x03) {
		case 0: DBG("Control Transfer Type\n");		break;
		case 1: DBG("Isochronous Transfer Type\n");	break;
		case 2: DBG("Bulk Transfer Type\n");			break;
		case 3: DBG("Interrupt Transfer Type\n");	break;
	}
	DBG("wMaxPacketSize:                  0x%04X\n", desc->wMaxPacketSize & 0x07FF);//, desc->wMaxPacketSize & 0x07FF);
	DBG("bInterval:                         0x%02X\n", desc->bInterval);
}

static void print_hiddesc(HIDDesc *desc)
{
	int i, tmp;
	DBG("\n          ===>HID Descriptor<===\n");
	DBG("bLength:                           0x%02X\n", desc->bLength);
	DBG("bDescriptorType:                   0x%02X\n", desc->bDescriptorType);
	DBG("bcdHID:                          0x%04X\n", desc->bcdHID);
	DBG("bCountryCode:                      0x%02X\n", desc->bCountryCode);
	DBG("bNumDescriptors:                   0x%02X\n", desc->bNumDescriptors);

	tmp = (desc->bLength-6) / 3;
	for(i=0; i<tmp; i++){
		DBG("bDescriptorType:                   0x%02X\n", desc->Report[i].bDescriptorType);
		DBG("wDescriptorLength:               0x%04X\n", desc->Report[i].wDescriptorLength);
	}
}

static void print_hubdesc(HubDesc *desc)
{
	DBG("\n          ===>HUB Descriptor<===\n");
	DBG("bDescLength:                       0x%02X\n", desc->bDescLength);
	DBG("bDescriptorType:                   0x%02X\n", desc->bDescriptorType);
	DBG("bNbrPorts:                         0x%02X\n", desc->bNbrPorts);
	DBG("wHubCharacteristics:             0x%04X\n", desc->wHubCharacteristics);
	DBG("bPwrOn2PwrGood:                    0x%02X = %d ms\n", desc->bPwrOn2PwrGood, desc->bPwrOn2PwrGood*2);
	DBG("bHubContrCurrent:                  0x%02X = %d mA\n", desc->bHubContrCurrent, desc->bHubContrCurrent);
	DBG("DeviceRemovable:                   0x%02X\n", desc->DeviceRemovable[0]);
	DBG("PortPwrCtrlMask:                   0x%02X\n", desc->PortPwrCtrlMask[0]);
}*/

/// <summary>
/// Sony SIXAXIS Initialize
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int Sony_SIXAXIS_Init(USBDevice *d)
{
	int i;
	
	_sixaxis_buf_index = 0;
	
	for( i=0; i<3; i++ ){
		if( Sony_SIXAXIS_Special_Request(d) >= 0 ){
			i = 0;
			break;
		}
	}
	if( i != 0 )	return -1;
	
	return USBAddIntTxTask(d, 1, 15, Sony_SIXAXIS_IntTask);
}

/// <summary>
/// Sony SIXAXIS Special Request
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
static int Sony_SIXAXIS_Special_Request(USBDevice *d)
{
	int ret;

	ret = Get_Report(
		d, 
		SIXAXIS_REPORT_TYPE, 
		_sixaxis_report_buf, 
		SIXAXIS_REPORT_LENGTH, 
		SIXAXIS_REPORT_ID, 
		d->intfdesc->bInterfaceNumber
	);
	return ret;
}

/// <summary>
/// Get Report
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="type">Report Type</param>
/// <param name="data">��M����f�[�^</param>
/// <param name="length">��M����f�[�^�̃T�C�Y</param>
/// <param name="report_id">Report ID</param>
/// <param name="intf">�C���^�[�t�F�[�X�ԍ�</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
int Get_Report(USBDevice *d, word type, byte *data, size_t length, byte report_id, word intf)
{
	CtrlReq command;
	
	command.bmRequestType = TRANS_TO_HOST | TRANS_TO_IF | TYPE_CLASS;
	command.bRequest = GET_REPORT;
	command.wValue = ((type << 8) | report_id);
	command.wIndex = intf;
	command.wLength = length;

	return CtrlTransfer(d, &command, data);
}

/// <summary>
/// USB Add Interrupt Transfers Task
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="ep">�G���h�|�C���g�ԍ��i1�`�j</param>
/// <param name="interval">�C���^���v�g�]�������Ԋu</param>
/// <param name="fp">�C���^���v�g�]�������n���h��</param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int USBAddIntTxTask(USBDevice *d, int ep, int interval, void (*fp)(USBDevice*))
{
	
	USBIntQueueItem	*newitem;
	int i;
	char flag;
	
	if( ep < 1 || ep > 15 )	return -1;
	
	flag = 0;
	for( i=0; i < d->intfdesc->bNumEndpoints; i++ ){
		if( (d->epdesc[i].bEndpointAddress & EPADDR_MASK_ADDRESS) == ep ){
			flag = 1;
			break;
		}
	}
	if( !flag || ((d->epdesc[i].bmAttributes) & 0x03 != 3) ||  (d->epdesc[i].bInterval == 0) )	return -1;

	newitem = &_newitem;
	
	newitem->device = d;
	newitem->ep = ep;
	newitem->handler = fp;
	newitem->interval = interval;
	newitem->cnt = interval;
	
	newitem->next = usbqueue;
	usbqueue = newitem;	// �|�C���^�̂Ȃ�����

	return 0;
}

/// <summary>
/// Sony SIXAXIS Interrupt Transfer Task
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
static void Sony_SIXAXIS_IntTask(USBDevice *d)
{
	int tmp;
	
	tmp = IntInTransfer(d, 1, &(_sixaxis_buf[0][_sixaxis_buf_index]), SIXAXIS_DATA_LENGTH);
}

/// <summary>
/// USB Interrupt IN Transfer
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="ep">�G���h�|�C���g�ԍ�</param>
/// <param name="data">��M����f�[�^</param>
/// <param name="size">��M����f�[�^�̃T�C�Y</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : HANDSHAKE_NAK [-1] �������� HANDSHAKE_STALL [-2]
/// </returns>
int IntInTransfer(USBDevice *d, int ep, byte *data, int size)
{
	d->devinfo.ep = ep;
	return SL811IntInTransfer(&(d->devinfo), data, size);
}

/// <summary>
/// SL811 USB Interrupt IN Transfer
/// </summary>
/// <param name="d">�Ώۃf�o�C�X</param>
/// <param name="data">��M����f�[�^</param>
/// <param name="size">��M����f�[�^�̃T�C�Y</param>
/// <returns>
/// ����I���� : ��M�����f�[�^�̃T�C�Y
/// �s���I���� : SL811_HANDSHAKE_NAK [-1] �������� SL811_HANDSHAKE_STALL [-2]
/// </returns>
int SL811IntInTransfer(SL811USBDevInfo *d, unsigned char *data, int size)
{
	d->rd_cmd = DATA0_RD;
	return SL811ReadData(d, data, size);
}

/// <summary>
/// Toggle SIXAXIS Recieve Buffer
/// </summary>
/// <returns>���݁i���O�j�̃o�b�t�@�ւ̃A�h���X</returns>
unsigned char *Toggle_SIXAXIS_Buffer(void)
{
	unsigned char *ret;
	
	ret = &(_sixaxis_buf[0][_sixaxis_buf_index]);
	_sixaxis_buf_index = (_sixaxis_buf_index == 1) ? 0 : 1;
	
	return ret;
}

/// <summary>
/// Toggle Remote Control Tx. Buffer
/// </summary>
/// <returns>���݁i���O�j�̃o�b�t�@�ւ̃A�h���X</returns>
union controller_data *Toggle_RC_Tx_Buffer(void)
{
	union controller_data *ret;
	
	ret = &(_rc_tx_buf[_rc_tx_buf_selector]);
	_rc_tx_buf_selector = (_rc_tx_buf_selector == 1) ? 0 : 1;
	_rc_tx_buf_index = 0;

	return ret;
}

void controler_main_process_loop(void)
{
	unsigned char status;
	unsigned char getinfosixaxis;
	int tmp, i;
//**/	int x;
	unsigned char L_STICK_X, L_STICK_Y, R_STICK_X, R_STICK_Y;
	
while(1)
	{
		// clear all interrupt bits
		for(status=0; status<20; status++)
		sl811_write(SL811H_INTSTATUSREG, 0xFF);

		for(getinfosixaxis = 0;getinfosixaxis != 0x00;)
		{
			getinfosixaxis = sl811_read(SL811H_INTSTATUSREG);
		}

		IntInTransfer(&dev, 1, &(_sixaxis_buf[0][_sixaxis_buf_index]), SIXAXIS_DATA_LENGTH);

		sixaxis = (SIXAXIS*)((Toggle_SIXAXIS_Buffer()) + 2);
		
//**/		printf("%d\n", sixaxis->Button.PS);
		if( sixaxis->Button.PS != 0 )	break;
		_delay_ms(50);
	}

	
//	uart_init(0,UART_RE|UART_TE,19200);						//��M����|���M����|���M�f�[�^�󊄂荞�݋��C�{�[���[�g
	//	uart_setbuffer(TxDataBuf,RC_DATA_LENGTH*7);				//���M�p�o�b�t�@�̐ݒ�
	MU2_SetRxHandler(uart0_getchar);						//uart��1byte��M�֐����Z�b�g
	MU2_SetTxHandler(uart0_putchar);						//uart��1byte���M�\��֐����Z�b�g������uart_putchar�� 
	
	for(i = 0;i<5; i++)
	{													//�����Ɗ��荞�݂ł͂Ȃ�wait�`���ɂȂ�			  	
		if((PINF&0x01)==1)
		{
//**/			printf("%x\n",SWITCH>>6);
			MU2_SetGroupID(Group[SWITCH>>6]);						//�O���[�v�ݒ�

//**/			printf("%x %x\n",SWITCH>>6,(SWITCH>>4) & 0x03);
			MU2_SetChannel(Channel[SWITCH>>6][(SWITCH>>4) & 0x03]);	//�`�����l���ݒ�

		}

		DELAY;
	}
	
	i = 0;
	LED_ON_ALL;

	// �����o�b�t�@�ւ̃|�C���^�擾
	txdata = Toggle_RC_Tx_Buffer();
	_delay_ms(100);

/*	uart0_putchar('@');
	uart0_putchar('B');
	uart0_putchar('R');
	uart0_putchar('4');
	uart0_putchar('8');
	uart0_putchar(0x0d);
	uart0_putchar(0x0a);
	_delay_ms(100);
	_delay_ms(100);
	_delay_ms(100);

	uart_init(0,UART_TE,4800);
	_delay_ms(100);
	_delay_ms(100);
	_delay_ms(100);
	_delay_ms(100);
	_delay_ms(100);
	_delay_ms(100);
*/	
	while(1)
	{
		if(i==0&&(PINF&0x01)==0) i = 1;
		if(i==1&&(PINF&0x01)==1)
		{
			_delay_ms(250);
			for(i = 0;i<5; i++)
			{
					MU2_SetGroupID(Group[SWITCH>>6]);						//�O���[�v�ݒ�
					//_delay_ms(100);
					MU2_SetChannel(Channel[SWITCH>>6][(SWITCH>>4) & 0x03]);	//�`�����l���ݒ�
					//_delay_ms(100);

				DELAY;
			}
			i = 0;

		}

		// clear all interrupt bits
		for(status=0; status<20; status++)
		sl811_write(SL811H_INTSTATUSREG, 0xFF);

		for(getinfosixaxis = 0;getinfosixaxis != 0x00;)
		{
			getinfosixaxis = sl811_read(SL811H_INTSTATUSREG);
		}

		IntInTransfer(&dev, 1, &(_sixaxis_buf[0][_sixaxis_buf_index]), SIXAXIS_DATA_LENGTH);

		sixaxis = (SIXAXIS*)((Toggle_SIXAXIS_Buffer()) + 2);
		
		// Button
		txdata->detail.Button.LEFT = sixaxis->Button.LEFT;
		txdata->detail.Button.DOWN = sixaxis->Button.DOWN;
		txdata->detail.Button.RIGHT = sixaxis->Button.RIGHT;
		txdata->detail.Button.UP = sixaxis->Button.UP;
		txdata->detail.Button.SQU = sixaxis->Button.SQU;
		txdata->detail.Button.BAT = sixaxis->Button.BAT;
		txdata->detail.Button.CIR = sixaxis->Button.CIR;
		txdata->detail.Button.TRI = sixaxis->Button.TRI;
		txdata->detail.Button.R1 = sixaxis->Button.R1;
		txdata->detail.Button.L1 = sixaxis->Button.L1;
		txdata->detail.Button.START = sixaxis->Button.START;
		txdata->detail.Button.SELECT = sixaxis->Button.SELECT;
		// Button
		
		// Analog Button
		if( sixaxis->AnalogButton.R2 < 100 )		tmp = 0;
		else if( sixaxis->AnalogButton.R2 < 180 )	tmp = 1;
		else if( sixaxis->AnalogButton.R2 < 255 )	tmp = 2;
		else										tmp = 3;
		txdata->detail.Button.R2 = tmp;
		
		if( sixaxis->AnalogButton.L2 < 100 )		tmp = 0;
		else if( sixaxis->AnalogButton.L2 < 180 )	tmp = 1;
		else if( sixaxis->AnalogButton.L2 < 255 )	tmp = 2;
		else										tmp = 3;
		txdata->detail.Button.L2 = tmp;
		// Analog Button
		
		// Analog Stick
		L_STICK_X = (sixaxis->AnalogL.X)/17;
		if(L_STICK_X == 15) L_STICK_X = 14;

		L_STICK_Y = (sixaxis->AnalogL.Y)/17;
		if(L_STICK_Y == 15) L_STICK_Y = 14;

		txdata->detail.AnalogL.X = L_STICK_X;
		txdata->detail.AnalogL.Y = L_STICK_Y;

		R_STICK_X = (sixaxis->AnalogR.X)/17;
		if(R_STICK_X == 15) R_STICK_X = 14;

		R_STICK_Y = (sixaxis->AnalogR.Y)/17;
		if(R_STICK_Y == 15) R_STICK_Y = 14;		

		txdata->detail.AnalogR.X = R_STICK_X;
		txdata->detail.AnalogR.Y = R_STICK_Y;
		// Analog Stick
		
//**/		for( x=0; x<7; x++ ){
//**/			printf("%02x ", txdata->buf[x]);
//**/		}
//		printf("%02x ",sixaxis->AnalogR.X);
//		printf("%02x ",sixaxis->AnalogR.Y);

//**/		printf("\n");
		
		txdata = Toggle_RC_Tx_Buffer();
		if((PINF&0x01)==1)
		{
			MU2_SendData(txdata->buf,RC_DATA_LENGTH);
		
		}
		else
		{
			MU2_SendDataBus(txdata->buf,RC_DATA_LENGTH);
		
		}
	_delay_ms(50);
	}

}
