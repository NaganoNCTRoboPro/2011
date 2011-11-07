#ifndef __REMOTE_CONTROL_RX_H__
#define __REMOTE_CONTROL_RX_H__

#define SIXAXIS (0)
#define NUNCHUK (1)
#define CLASSIC (2)

#define CONTROLLER CLASSIC

#if CONTROLLER == SIXAXIS
	#include "rc_sixaxis.h"
#elif CONTROLLER == NUNCHUK
	#include "rc_nunchuk.h"
#elif CONTROLLER == CLASSIC
	#include "rc_classic.h"
#endif

#include <avr/io.h>
#include <stdbool.h>


void RC_Rx_Init(void);
union controller_data *Toggle_RC_Rx_Buffer(void);
void initCtrlData(void);
void initCommunicateIntervalCounter(void);

#endif
