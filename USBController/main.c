#include <avr/io.h>
#include <avr/interrupt.h>
#include <wait.h>
#include <usb.h>
#include <sl811.h>
#include <sony_sixaxis.h>
#include <rc_sixaxis.h>
#include <uart.h>
#include <mu2.h>
#include <led.h>

#define VBUS_ON PORTD &= ~_BV(PORTD7)
#define VBUS_OFF PORTD |= _BV(PORTD7)

void initialize(void);
void mem_init(void);
int Sony_SIXAXIS_Special_Request(USBDevice *d);
int Sony_SIXAXIS_Init(USBDevice *d);
void Sony_SIXAXIS_IntTask(USBDevice *d);
unsigned char *Toggle_SIXAXIS_Buffer(void);

unsigned char _sixaxis_buf[SIXAXIS_DATA_LENGTH][2];
unsigned char _sixaxis_buf_index;
unsigned char _sixaxis_report_buf[SIXAXIS_REPORT_LENGTH];

int main(void)
{
	int16_t error,speed;
	uint8_t i;
	uint8_t ad;
	uint8_t errorCount;

	SIXAXIS *sixaxis;

	union controller_data txdata;

	USBDevice dev;

	unsigned char L_STICK_X, L_STICK_Y, R_STICK_X, R_STICK_Y;
	 
	initialize();
	mem_init();
	uart_init(0,UART_TE|UART_RE,BR_19200);

RESET:
	do {
		LED_OFF_ALL;
		cli();	
		LED0_ON;

		VBUS_ON;
		wait_ms(1000);
		wait_ms(100);

		error = USBInit();
	} while( error );

	speed = SL811USBReset();
	if ( speed == USB_LOW || speed == USB_FULL )
		{
				LED_OFF_ALL;
				LED1_ON;
				
				while ( USBDeviceInit_in3(&dev, NULL, speed) )
					{
						wait_ms(100);
					}
				LED_OFF_ALL;
				LED0_ON;
				LED1_ON;
				wait_ms(100);
				errorCount = 0;
				do {
					_sixaxis_buf_index = 0;
					error =  Sony_SIXAXIS_Init(&dev);
					if( ++errorCount > 5 ) goto RESET;
					wait_ms(250);
				} while( error );
		}
	while(1) 
		{
			for ( i=0; i<20; i++ )
				sl811_write(SL811H_INTSTATUSREG, 0xFF);
			
			for ( ; error; )
				error = sl811_read(SL811H_INTSTATUSREG);

			IntInTransfer(	&dev, 
							1, 
							&(_sixaxis_buf[0][_sixaxis_buf_index]), 
							SIXAXIS_DATA_LENGTH
						  );
			sixaxis = (SIXAXIS*)((Toggle_SIXAXIS_Buffer()) + 2);

			if( sixaxis->Button.PS != 0 )	break;
		}
	LED_ON_ALL;
	while(1)
		{
			for ( i=0; i<20; i++ )
				sl811_write(SL811H_INTSTATUSREG, 0xFF);
			
			for ( ; error; )
				error = sl811_read(SL811H_INTSTATUSREG);

			IntInTransfer(	&dev, 
							1, 
							&(_sixaxis_buf[0][_sixaxis_buf_index]), 
							SIXAXIS_DATA_LENGTH
						  );
			sixaxis = (SIXAXIS*)((Toggle_SIXAXIS_Buffer()) + 2);

			txdata.detail.Button.LEFT = sixaxis->Button.LEFT;
			txdata.detail.Button.DOWN = sixaxis->Button.DOWN;
			txdata.detail.Button.RIGHT = sixaxis->Button.RIGHT;
			txdata.detail.Button.UP = sixaxis->Button.UP;
			txdata.detail.Button.SQU = sixaxis->Button.SQU;
			txdata.detail.Button.BAT = sixaxis->Button.BAT;
			txdata.detail.Button.CIR = sixaxis->Button.CIR;
			txdata.detail.Button.TRI = sixaxis->Button.TRI;
			txdata.detail.Button.R1 = sixaxis->Button.R1;
			txdata.detail.Button.L1 = sixaxis->Button.L1;
			txdata.detail.Button.START = sixaxis->Button.START;
			txdata.detail.Button.SELECT = sixaxis->Button.SELECT;

			if( sixaxis->AnalogButton.R2 < 100 )		ad = 0;
			else if( sixaxis->AnalogButton.R2 < 180 )	ad = 1;
			else if( sixaxis->AnalogButton.R2 < 255 )	ad = 2;
			else										ad = 3;
			txdata.detail.Button.R2 = ad;
		
			if( sixaxis->AnalogButton.L2 < 100 )		ad = 0;
			else if( sixaxis->AnalogButton.L2 < 180 )	ad = 1;
			else if( sixaxis->AnalogButton.L2 < 255 )	ad = 2;
			else										ad = 3;
			txdata.detail.Button.L2 = ad;
			// Analog Button
		
			// Analog Stick
			L_STICK_X = (sixaxis->AnalogL.X)/17;
			if(L_STICK_X == 15) L_STICK_X = 14;

			L_STICK_Y = (sixaxis->AnalogL.Y)/17;
			if(L_STICK_Y == 15) L_STICK_Y = 14;

			txdata.detail.AnalogL.X = L_STICK_X;
			txdata.detail.AnalogL.Y = L_STICK_Y;

			R_STICK_X = (sixaxis->AnalogR.X)/17;
			if(R_STICK_X == 15) R_STICK_X = 14;

			R_STICK_Y = (sixaxis->AnalogR.Y)/17;
			if(R_STICK_Y == 15) R_STICK_Y = 14;		

			txdata.detail.AnalogR.X = R_STICK_X;
			txdata.detail.AnalogR.Y = R_STICK_Y;
			// Analog Stick

			MU2_SendData(txdata.buf,RC_DATA_LENGTH);

		}

	return 0;
}

int Sony_SIXAXIS_Init(USBDevice *d)
{
	uint8_t i;
	
	for( i=0; i<3; i++ ){
		if( Sony_SIXAXIS_Special_Request(d) >= 0 ){
			i = 0;
			break;
		}
	}
	if( i != 0 )	return -1;
	
	return USBAddIntTxTask(d, 1, 15, Sony_SIXAXIS_IntTask);
}

/**
 * @param {d} 対象デバイス
 * @return 正常終了時 : 受信したデータのサイズ
 *         不正終了時 : HANDSHAKE_NAK [-1] もしくは HANDSHAKE_STALL [-2]
 */
SWord Sony_SIXAXIS_Special_Request(USBDevice *d)
{
	SWord ret;

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
/**
 * Sony SIXAXIS SWorderrupt Transfer Task
 *
 * @param {d} 対象デバイス
 */
void Sony_SIXAXIS_IntTask(USBDevice *d)
{
	IntInTransfer(d, 1, &(_sixaxis_buf[0][_sixaxis_buf_index]), SIXAXIS_DATA_LENGTH);
}
unsigned char *Toggle_SIXAXIS_Buffer(void)
{
	unsigned char *ret;
	
	ret = &(_sixaxis_buf[0][_sixaxis_buf_index]);
	_sixaxis_buf_index = (_sixaxis_buf_index == 1) ? 0 : 1;
	
	return ret;
}
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
