#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

#include <avr/io.h>
#include "common.h"

#define OCR1A_VALUE  2500// 20MHz , 8������

/// <summary>
/// 1ms�Ԋu�ŋN������V�X�e���^�C�}�[�̏�����
/// </summary>
extern __inline__ void SystemTimerInit(void)
{
	unsigned short temp = OCR1A_VALUE;

	TCCR1A = 0x0c;		// OCR1A�ŃN���A
	TCCR1B = 0x0a;		// OCR1A�ŃN���A , �N���b�N8����
	
	OCR1AH = (unsigned char)(temp>>8);
	OCR1AL = (unsigned char)temp;

	sbi(TIMSK1,OCIE1A);	// OCR1A��r��v���荞�݋���
}

extern __inline__ void SystemTimerIntvalProcess(void)
{
	sbi(TIFR1,OCF1A);	// �t���O�N���A(�_��1�̏������݂ŃN���A�����)
}

#endif
