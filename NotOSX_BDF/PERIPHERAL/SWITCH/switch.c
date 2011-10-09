#include "switch.h"

void SwitchInit(void)
{
	DDRB &= 0xf0;
	PORTB |= 0x0f;
}

/// <summary>
/// �X�C�b�`�l�擾�֐� 
/// </summary>
/// <param name="target"> ���C����Տ�̃X���C�h�X�C�b�`�ԍ��@1�`4 </param>
/// <return> �X�C�b�`�̏�� ON -> ture , OFF -> false </return>
bool Switch(unsigned char target)
{
	unsigned char status;
	
	status = PINB ^ 0xff;
	status >>= (target-1); 

	return (bool)(status & 0x01);
}

__inline__ unsigned char Group(void)
{
	return ((PINB ^ 0xff)>>2)&0x03;
}

__inline__ unsigned char Channel(void)
{
	return (PINB ^ 0xff)&0x03;
}

