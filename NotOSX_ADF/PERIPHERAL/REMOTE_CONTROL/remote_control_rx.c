#include "remote_control_rx.h"
#if CONTROLLER == SIXAXIS
	#include "rc_sixaxis.h"
#elif CONTROLLER == NUNCHUK
	#include "rc_nunchuk.h"
#elif CONTROLLER == CLASSIC
	#include "rc_classic.h"
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <common.h>
//#include <kernel.h>
//#include <kernel_setting.h>
#include <uart.h>
//#include <ringbuffer.h>
#include <mu2.h>
#include <led.h>
#include <beep.h>
#include <emergency.h>

static union controller_data ctrlData,keepCtrlData;
//static uint8_t defaultCtrlData[RC_DATA_LENGTH] = RC_DEFAULT_DATA;  

//static uint8_t rxBuf[RC_DATA_LENGTH+MU2_OFFSET];
//static RingBuffer *ring = &__uartbuf[0];
static volatile uint8_t packet[24];
static uint8_t val;
static volatile uint8_t i=0,cnt=0;
static volatile bool phase = false;
static volatile char check[] = "DR=";
/*

TCB remoteCrtl;
Stack remoteCrtlStack[DEFAULT_STACK_SIZE];

void decodeCtrlData(void);

void RC_Rx_Init(void)
{
	uint8_t i;
	for(i=0;i<RC_DATA_LENGTH;i++){
		ctrlData.buf[i] = defaultCtrlData[i];
	}
	remoteCrtl.NextTask = NULL;
	remoteCrtl.Status = READY;
	remoteCrtl.Priority = 1;
	remoteCrtl.pTask = &decodeCtrlData;
	remoteCrtl.pStack = remoteCrtlStack + DEFAULT_STACK_SIZE;
	remoteCrtl.StackSize = DEFAULT_STACK_SIZE;
	remoteCrtl.ID = 1;

	RingInit(ring,rxBuf,RC_DATA_LENGTH+MU2_OFFSET);

	AddTaskList(&remoteCrtl);
}

void decodeCtrlData(void)
{
	while(1){
		while(RingGet(&(__uartbuf[0]),&val)){
			//uart1_putchar(val);
			if(phase){
				packet[cnt] = val;
				uart1_putchar(packet[cnt]);
				if(cnt<2){
					if(val != check[cnt]){
						phase = false;
						uart1_putchar('@');
					}
				}else if(cnt==10){
					ctrlData.buf[0] = packet[5];
					ctrlData.buf[1] = packet[6];
					ctrlData.buf[2] = packet[7];
					ctrlData.buf[3] = packet[8];
					phase = false;
				}
				cnt++;
			}else{
				if(val=='*'){
				cnt = 0;
				phase = true;
				}
			}

		}
		LED(0,false);
		Sleep(5);
	}
}
*/
union controller_data *Toggle_RC_Rx_Buffer(void)
{
	cli();
	for(i=0;i<RC_DATA_LENGTH;i++){
		keepCtrlData.buf[i] = ctrlData.buf[i];
	}
	sei();
	return &keepCtrlData;
}


ISR(USART0_RX_vect)
{
	val = UDR0;

	if(phase){
		packet[cnt] = val;
		uart1_putchar(packet[cnt]);
		if(cnt<2){
			if(val != check[cnt]){
				phase = false;
				uart1_putchar('@');
				LED(0,false);
				beep(1);
			}
		}else if(cnt==10){
			ctrlData.buf[0] = packet[5];
			ctrlData.buf[1] = packet[6];
			ctrlData.buf[2] = packet[7];
			ctrlData.buf[3] = packet[8];
			phase = false;
			wdt_reset();
			LED(0,true);
			if(EmergencyStatus()) beep(0);
		}
		cnt++;
	}else{
		if(val=='*'){
		cnt = 0;
		phase = true;
		}
	}

}
