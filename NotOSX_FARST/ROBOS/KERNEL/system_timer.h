#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

#include <avr/io.h>
#include "common.h"

#define OCR1A_VALUE  2500// 20MHz , 8分周時

/// <summary>
/// 1ms間隔で起動するシステムタイマーの初期化
/// </summary>
extern __inline__ void SystemTimerInit(void)
{
	unsigned short temp = OCR1A_VALUE;

	TCCR1A = 0x0c;		// OCR1Aでクリア
	TCCR1B = 0x0a;		// OCR1Aでクリア , クロック8分周
	
	OCR1AH = (unsigned char)(temp>>8);
	OCR1AL = (unsigned char)temp;

	sbi(TIMSK1,OCIE1A);	// OCR1A比較一致割り込み許可
}

extern __inline__ void SystemTimerIntvalProcess(void)
{
	sbi(TIFR1,OCF1A);	// フラグクリア(論理1の書き込みでクリアされる)
}

#endif
