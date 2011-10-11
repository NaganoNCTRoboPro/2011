#include "mu2_common.h"

int (*__mu2_tx_char)(unsigned char);
int (*__mu2_rx_char)(unsigned char *);
void (*__mu2_mode)(int);
int (*__mu2_rts)(void);
void (*__mu2_cts)(int);

char __mu2_rx_buf[MU2_RX_BUF_SIZE];
char __mu2_prefix[3];

__inline__ int MU2_TxChar(unsigned char c){ return (*__mu2_tx_char)(c);	}
__inline__ int MU2_RxChar(unsigned char *c){ return (*__mu2_rx_char)(c); }


/// <summary>
/// MU-2 1バイト送信関数の設定
/// </summary>
/// <param name="fp">セット対象の関数</param>
void MU2_SetTxHandler(int (*fp)(unsigned char))
{
	__mu2_tx_char = fp;
}

/// <summary>
/// MU-2 1バイト受信関数の設定
/// </summary>
/// <param name="fp">セット対象の関数</param>
void MU2_SetRxHandler(int (*fp)(unsigned char *))
{
	__mu2_rx_char = fp;
}

/// <summary>
/// MU-2 MODE端子操作関数の設定
/// </summary>
/// <param name="fp">セット対象の関数</param>
void MU2_SetModeHandler(void (*fp)(int))
{
	__mu2_mode = fp;
}

/// <summary>
/// MU-2 RTS端子操作関数の設定
/// </summary>
/// <param name="fp">セット対象の関数</param>
void MU2_SetRtsHandler(int (*fp)(void))
{
	__mu2_rts = fp;
}

/// <summary>
/// MU-2 CTS端子操作関数の設定
/// </summary>
/// <param name="fp">セット対象の関数</param>
void MU2_SetCtsHandler(void (*fp)(int))
{
	__mu2_cts = fp;
}

/// <summary>
/// バイト（0x00?0xFF）の数を2桁の文字列に変換
/// </summary>
/// <param name="num">変換対象の数字</param>
/// <param name="str">変換後の文字列を格納する領域</param>
void MU2_Byte2Str(unsigned char num, char *str)
{
	int tmp;

	tmp = (num >> 4) & 0x0F;
	str[0] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	tmp = num & 0x0F;
	str[1] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	str[2] = '\0';
}

/// <summary>
/// 2桁の16進数で表された数字を unsigned char 型に変換
/// </summary>
/// <param name="num">変換対象の数字</param>
/// <param name="str">変換後の文字列を格納する領域</param>
unsigned char MU2_Str2Byte(char *str)
{
	unsigned char tmp;

	tmp = str[0] - ((str[0] >= 'A') ? ('A'-10) : '0' );
	tmp <<= 4;
	tmp |= str[1] - ((str[1] >= 'A') ? ('A'-10) : '0' );

	return tmp;
}

/// <summary>
/// バイト（0x0000?0xFFFF）の数を4桁の文字列に変換
/// </summary>
/// <param name="num">変換対象の数字</param>
/// <param name="str">変換後の文字列を格納する領域</param>
void MU2_Word2Str(unsigned short num, char *str)
{
	int tmp;

	tmp = (num >> 12) & 0x0F;
	str[0] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	tmp = (num >> 8) & 0x0F;
	str[1] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	tmp = (num >> 4) & 0x0F;
	str[2] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	tmp = num & 0x0F;
	str[3] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	str[4] = '\0';
}

/// <summary>
/// 4桁の16進数で表された数字を unsigned short 型に変換
/// </summary>
/// <param name="num">変換対象の数字</param>
/// <param name="str">変換後の文字列を格納する領域</param>
unsigned short MU2_Str2Word(char *str)
{
	unsigned short tmp;

	tmp = str[0] - ((str[0] >= 'A') ? ('A'-10) : '0' );
	tmp <<= 4;
	tmp |= str[1] - ((str[1] >= 'A') ? ('A'-10) : '0' );
	tmp <<= 4;
	tmp |= str[2] - ((str[2] >= 'A') ? ('A'-10) : '0' );
	tmp <<= 4;
	tmp |= str[3] - ((str[3] >= 'A') ? ('A'-10) : '0' );

	return tmp;
}

/// <summary>
/// コマンド名のチェック
/// </summary>
/// <param name="prefix1">Prefix 1</param>
/// <param name="prefix2">Prefix 2</param>
/// <returns>
/// OK : 0
/// NG : -1
/// </returns>
int MU2_CheckCommand(char *prefix1, char *prefix2)
{
	if( (prefix1[0] == prefix2[0]) && (prefix1[1] == prefix2[1]) ){
		return 0;
	}else{
		return -1;
	}
}

/// <summary>
/// MU-2 コマンド
/// </summary>
/// <param name="cmd">コマンド名</param>
/// <param name="value">バリュー</param>
/// <returns>
/// 正常終了時 : 0
/// 不正終了時 : -1
/// </returns>
int MU2_Command(
	char *cmd,
	char *value1,
	unsigned char value1_len,
	char *value2,
	unsigned char value2_len,
	char *value3,
	unsigned char value3_len)
{

	// Prefix
	MU2_TxChar('@');

	// Command Name
	MU2_TxChar( cmd[0] );
	MU2_TxChar( cmd[1] );

	// Value 1
	while( value1_len-- )	MU2_TxChar( *value1++ );

	// Value 2
	while( value2_len-- )	MU2_TxChar( *value2++ );

	// Value 3
	while( value3_len-- )	MU2_TxChar( *value3++ );

	// Terminator
	MU2_TxChar('\r');	MU2_TxChar('\n');

	return 0;
}


/// <summary>
/// MU-2 レスポンス取得
/// </summary>
/// <param name="prefix">プレフィックス</param>
/// <param name="value">バリュー</param>
/// <returns>
/// 正常終了時 : 0
/// 不正終了時 : -1
/// </returns>
int MU2_Response(char *prefix, char *value)
{
	unsigned char c = '\0';

	// Prefix
	while( c != '*')	MU2_RxChar( &c );

	// Command Name
	MU2_RxChar( &c );
	prefix[0] = c;
	MU2_RxChar( &c );
	prefix[1] = c;
	prefix[2] = '\0';

	// Equal '='
	MU2_RxChar( &c );

	// Value
	while( 1 ){
		MU2_RxChar( &c );
		if( c == '\r' ){
			MU2_RxChar( &c );
			if( c == '\n'){
				*value = '\0';
				return 0;
			}else{
				*value = '\r';
				value++;
			}
		}
		*value = c;
		value++;
	}
}
