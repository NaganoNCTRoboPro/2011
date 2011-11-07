#include <avr/io.h>
#include <stdbool.h>
#include "mu2.h"

/**
 * シリアル送信関数用ポインタ
 * @type {void(*)(void)} 
 */
static void (*put)(uint8_t c);
/**
 * シリアル受信用関数ポインタ
 * @type {uint8_t(*)(void)}
 */
static uint8_t (*get)(void);

/**
 * シリアル送信関数用ポインタセッター関数
 * @param {f:void(*)(void)}
 */
void setMU2PutFunc(void(*f)(uint8_t c))
{
	put = f;
}
/**
 * シリアル受信関数用ポインタセッター関数
 * @param {f:uint8_t(*)(void)}
 */
void setMU2GetFunc(uint8_t(*f)(void))
{
	get = f;
}

/**
 * 16新変換用テーブル
 * @type {const char}
 */
static const char ascii[16] = "0123456789ABCDEF";

/**
 * 1Byte->16進文字列変換関数
 * @param {byte:uint8_t} 変換対象データ
 * @param {string:char*} 変換後文字列格納先ポインタ
 */
void byteToString(uint8_t byte, char* string)
{
	string[0] = ascii[ ( byte >> 4 ) & 0x0F ];
	string[1] = ascii[ byte & 0x0F ];
	string[2] = '\0';
}

/**
 * MU2にコマンドを送信する関数
 * @param {command:const char*} MU2に送信するコマンド
 * @param {values:uint8_t*} コマンドに付加するデータ
 */
bool mu2Command(const char* command, const char* values)
{
	uint8_t i, recv[10];
	
	put('@');
	put(command[0]);
	put(command[1]);
	for ( ; *values != '\0'; values++ )
		{
			put(*values);
		}
	put(0x0d);
	put(0x0a);
	
	for ( i = 0; i < 10; i++ )
		{
			recv[i] = get();
			if( i > 0 && recv[i-1] == 0x0d && recv[i] == 0x0a )
				{
					break;
				}
		}
	if ( recv[1] == 'E' && recv[2] == 'R' )
		{
			return true;
		}
	
	return false;
}
/**
 * MU2にコマンドを送信し，MU2のEEPROMに書きこむ関数
 * @param {command:const char*} MU2に送信するコマンド
 * @param {values:uint8_t*} コマンドに付加するデータ
 */
bool mu2CommandToEEPROM(const char* command, const char* values)
{
	uint8_t i, recv[10];
	
	put('@');
	put(command[0]);
	put(command[1]);
	for ( ; *values != '\0'; values++ )
		{
			put(*values);
		}
	put('/');
	put('W');
	put(0x0d);
	put(0x0a);
	
	for ( i = 0; i < 10; i++ )
		{
			recv[i] = get();
			if( i > 0 && recv[i-1] == 0x0d && recv[i] == 0x0a )
				{
					break;
				}
		}
	if ( recv[1] == 'E' && recv[2] == 'R' )
		{
			return true;
		}
	
	return false;
}

/**
 * MU2でデータを送信する関数
 * @param {command:const char*} MU2に送信するコマンド
 * @param {values:uint8_t*} コマンドに付加するデータ
 */
void mu2TransmitData(uint8_t* values, uint8_t size)
{
	uint8_t i;
	char strSize[3];
	byteToString(size,strSize);
	put('@');
	put('D');
	put('T');
	put(strSize[0]);
	put(strSize[1]);
	for ( i = 0; i < size; i++ )
		{
			put(values[i]);
		}
	put(0x0d);
	put(0x0a);
}
