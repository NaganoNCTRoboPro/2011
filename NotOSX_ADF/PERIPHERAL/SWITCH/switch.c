#include "switch.h"

void SwitchInit(void)
{
	DDRB &= 0xf0;
	PORTB |= 0x0f;
}

/// <summary>
/// スイッチ値取得関数 
/// </summary>
/// <param name="target"> メイン基盤上のスライドスイッチ番号　1～4 </param>
/// <return> スイッチの状態 ON -> ture , OFF -> false </return>
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

