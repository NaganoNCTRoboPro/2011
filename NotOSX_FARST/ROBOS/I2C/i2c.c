#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "i2c.h"
#include "i2c_private.h"

#if MASTER_COMPILE

/**
 * TWIをマスターとして初期化する
 * @example
 *     initI2CMaster(100); //100kbpsでマスター動作に初期化
 *
 * @param {speed:uint16_t} I2Cのバススピード
 */
void initI2CMaster(uint16_t speed) 
{
	// ボーレートを設定し，TWI動作を許可する
		TWBR = ( ( ( FREQ * 1000 ) / speed ) - 16 ) / 2;
		TWSR = 0;
		TWCR = EnableI2C;
}

/**
 * マスターがスレーブにwriteする
 * @example
 *     uint8_t transBuf = 0, recvBuf = 0;
 *     Slave slave = { 0x01, { &transBuf, 1 }, { &recvBuf, 1 } };
 *     transBuf = 0xff;
 *     i2cWrite(&slave);   // アドレスが0x01のスレーブに0xffが書かれる
 *
 * @param {slave:Slave*} 送信先のスレーブインスタンスのポインタ
 */
bool i2cWrite(Slave* slave)
{
		uint8_t i, size;
	// 開始条件の生成
		TWCR = SendStartConditionBit;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentStartConditionBit ) goto ERROR;
		
	// アドレス転送，ACK確認		
		TWDR = ( slave->address << 1 ) | WRITE;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentSlaveAddressWithWriteBitAndACK ) goto ERROR;
		
	// データバイト転送，ACK確認の一連をデータサイズ分行う
		for ( i = 0, size = slave->write.size; i < size; i++ )
			{
				TWDR = slave->write.buffer[i];
				TWCR = ContinueI2C;
				while ( I2C_DOING );
				if ( I2C_STATUS != SentDataByteAndACKAtMaster ) goto ERROR;
			}
		
	// 終了条件の生成
		TWCR = SendStopConditionBit;
		
	// 正常終了
		return false;
		
	// エラーケース
ERROR:
		TWCR = SendStopConditionBit;
		return true;
}

/**
 * マスターがスレーブにreadする
 * @example
 *     uint8_t transBuf = 0, recvBuf = 0;
 *     Slave slave = { 0x01, { &transBuf, 1 }, { &recvBuf, 1 } };
 *     i2cRead(&slave);   // アドレスが0x01のスレーブから値を1byte読む
 *
 * @param {slave:Slave*} 受信先のスレーブインスタンスのポインタ
 */
bool i2cRead(Slave* slave)
{
		uint8_t i, size;
	// 開始条件の生成
		TWCR = SendStartConditionBit;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentStartConditionBit ) goto ERROR;
		
	// アドレス転送，ACK確認
		TWDR = ( slave->address << 1 ) | READ;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentSlaveAddressWithReadBitAndACK ) goto ERROR;
		
	// データ受信，ACK発行
		for ( i = 0, size = slave->read.size - 1; i < size; i++ )
			{
				TWCR = ContinueAndResponseI2C;
				while( I2C_DOING );
				if ( I2C_STATUS != ReceivedDataByteAndACKAtMaster ) goto ERROR;
				slave->read.buffer[i] = TWDR;
			}
			
	// 最終データ受信，NACK発行
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != ReceivedDataByteAndNACK ) goto ERROR;
		slave->read.buffer[size] = TWDR;
		
	// 終了条件の生成
		TWCR = SendStopConditionBit;
		
	// 正常終了
		return false;
		
	// エラーケース
ERROR:
		TWCR = SendStopConditionBit;
		return true;
}

/**
 * マスターがスレーブにコマンドを送ってからreadする
 * @example
 *     uint8_t transBuf = 0, recvBuf = 0;
 *     Slave slave = { 0x01, { &transBuf, 1 }, { &recvBuf, 1 } };
 *     i2cReadWithCommand(&slave,cmd);   // アドレスが0x01のスレーブにcmdを送り，つづけて値を1byte読む
 *
 * @param {slave:Slave*} 送受信先のスレーブインスタンスのポインタ
 * @param {command:uint8_t} 送信コマンド
 */
bool i2cReadWithCommand(Slave* slave, uint8_t command)
{
		uint8_t i, size;
	// 開始条件の生成
		TWCR = SendStartConditionBit;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentStartConditionBit ) goto ERROR;
    
	// アドレス転送
		TWDR = ( slave->address << 1 ) | READ;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentSlaveAddressWithReadBitAndACK ) goto ERROR;
    
	// コマンド転送，ACK確認
		TWDR = command;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentDataByteAndACKAtMaster ) goto ERROR;
    
	// データバイト受信，ACK発行の一連をデータサイズ-1分行う
		for ( i = 0, size = slave->read.size - 1; i < size; i++ )
		{
		    TWCR = ContinueAndResponseI2C;
		    while( I2C_DOING );
		    if ( I2C_STATUS != ReceivedDataByteAndACKAtMaster ) goto ERROR;
		    slave->read.buffer[i] = TWDR;
		}
    
	// 最終データバイト受信，NACK発行
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != ReceivedDataByteAndNACK ) goto ERROR;
		slave->read.buffer[size] = TWDR;
    
	// 終了条件の生成
		TWCR = SendStopConditionBit;
    
	// 正常終了
		return false;
    
	// エラーケース
ERROR:
		TWCR = SendStopConditionBit;
		return true;
}
#endif // MASTER_COMPILE


#if SLAVE_COMPILE

/**
 * スレーブで割り込み時に使うグローバル変数の宣言
 */
static volatile Slave* own;
static volatile uint8_t byteCount;
static volatile bool emergency = false;
static volatile bool i2cComFlag = false;	

/**
 * TWIをスレーブとして初期化する
 * @example
 *     uint8_t recvBuf = 0, transBuf = 0;
 *     //マスター視点のwrite，readなのでスレーブだとwriteは受信，readは送信扱いになる
 *     Slave own = { 0x01, { &recvBuf, 1 }, { &transBuf, 1 } };
 *     initI2CSlave(&own);   // アドレスが0x01のスレーブに自身がセットされる
 *     sei();
 *
 * @param {_own:Slave*} 自分自身のスレーブインスタンスのポインタ
 */
void initI2CSlave(Slave* _own) 
{
	// 自アドレスを設定し，TWI，ACK，割り込み，一斉呼出しを許可する
    own = _own;
    TWAR = own->address << 1;
    TWAR |= GCRE_BIT;		
    TWCR = ACKEnableI2C | EnableI2C | InterruptEnableI2C;
}

/**
 * 非常事態検査関数
 * @return {bool} 非常時か否か
 */

bool isEmergency(void)
{
	return emergency;
}


/**
 * 通信中検査関数
 * @return {bool} 通信中か否か
 */
bool isCommunicatingViaI2C(void)
{
	return i2cComFlag;
}


/**
 * スレーブ動作の時の割り込み処理
 */
ISR (TWI_vect)
{
    switch (TWSR)
        {
            // 一斉呼出受信，非常時と認定，breakなし
                case ReceivedGeneralCallRecognitionAndACK:
                    emergency = true;
                
            // 自アドレス+W受信，TWINTを下ろす
                case ReceivedOwnAddressWithWriteBitAndACK:
                    byteCount = 0;
                    i2cComFlag = true;
                    TWCR |= ContinueI2C;
                    break;
                
            // 一斉呼出データバイト受信，サポートしない
                case ReceivedGeneralCallRecognitionDataByteAndACK:
                    TWCR |= ContinueI2C;
                    break;
                
            // データバイト受信，バッファに移してTWINTを下ろす
                case ReceivedDataByteAndACKAtSlave:
                    own->write.buffer[byteCount++] = TWDR;
                    break;
                
            // 終了条件の受信，TWINTを下ろす
                case ReceivedStopConditionBit:
                    TWCR |= ContinueI2C;
                    break;
                
            // 自アドレス+R受信，1バイト目のデータバイトの送信を行い，TWINTを下ろす
                case ReceivedOwnAddressWithReadBitAndACK:
                    byteCount = 0;
                    i2cComFlag = true;
                    TWDR = own->read.buffer[byteCount++];				
                    TWCR |= ContinueI2C;
                    break;
                
            // ACK受信，データバイトの送信を行い，TWINTを下ろす
                case SentDataByteAndACKAtSlave:
                    TWDR = own->read.buffer[byteCount++];
                    TWCR |= ContinueI2C;
                    break;
                
            // NACKの受信，TWINTを下ろす
                case SentDataByteAndNACKAtSlave:
                    TWCR |= ContinueI2C;
                    i2cComFlag = false;
                    break;
                
            // 他，TWCRの初期化
                default:
                    TWCR = ACKEnableI2C | EnableI2C | InterruptEnableI2C;
                    i2cComFlag = false;
                    break;
        }
}
#endif // SLAVE_COMPILE
