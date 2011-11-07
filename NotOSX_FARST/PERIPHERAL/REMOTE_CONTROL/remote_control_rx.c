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
#include <uart.h>
#include <mu2.h>
#include <led.h>
#include <beep.h>
#include <emergency.h>

static union controller_data ctrlData,keepCtrlData;
static const uint8_t defaultCtrlData[RC_DATA_LENGTH] = RC_DEFAULT_DATA;  

static volatile uint8_t packet[24];
static uint8_t val;
static volatile uint8_t i=0,cnt=0;
static volatile bool phase = false;
static volatile char check[] = "DR=";
static volatile uint8_t ovf_cnt = 0;

__inline__ void resetCommunicateIntervalCounter(void);

void initCtrlData(void){
	for(i=0;i<RC_DATA_LENGTH;i++){
			ctrlData.buf[i] = defaultCtrlData[i];
		}
}
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
		if(cnt<2){
			if(val != check[cnt]){
				phase = false;
				LED(0,false);
				beep(1);
			}
		}else if(cnt==8){
			ctrlData.buf[0] = packet[5];
			ctrlData.buf[1] = packet[6];
			ctrlData.buf[2] = packet[7];
			ctrlData.buf[3] = packet[8];
			phase = false;
			wdt_reset();
			LED(0,true);
			if(EmergencyStatus()) beep(0);
			resetCommunicateIntervalCounter();
		}
		cnt++;
	}else{
		if(val=='*'){
			cnt = 0;
			phase = true;
		}
	}

}

void resetCommunicateIntervalCounter(void)
{
	ovf_cnt = 0;
	TCNT1 = 0;
}

__inline__ void initCommunicateIntervalCounter(void)
{
	TCCR1A = 0;
	TCCR1B = 5;
	TIMSK1 = 1;
	TCNT1 = 0;
}

ISR (TIMER1_OVF_vect){
	if(++ovf_cnt>150){
		ovf_cnt = 0;
		for(i=0;i<RC_DATA_LENGTH;i++){
			ctrlData.buf[i] = defaultCtrlData[i];
		}
	}
}
