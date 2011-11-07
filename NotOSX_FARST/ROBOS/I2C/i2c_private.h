#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

// GCRE = General Call Recognition Enable = 一斉呼出
#define GCRE_BIT 	(1)
#define I2C_DOING	( ! ( TWCR & _BV(TWINT) ) )
#define I2C_STATUS	( TWSR & 0xF8 )
#define WRITE		(0)
#define READ		(1)

/**
 * TWCRに設定する値群
 */
enum I2CControl {
    // I2Cの動作を許可する
    	EnableI2C = _BV(TWEN),
    
    // I2Cの割り込みを許可する
    	InterruptEnableI2C = _BV(TWIE),
    
    // ACK応答を許可する
    	ACKEnableI2C = _BV(TWEA),
    
    // I2Cの動作を続ける
    	ContinueI2C = _BV(TWEN) | _BV(TWINT),
    
    // I2Cの動作を続ける．かつ，応答を返す
    	ContinueAndResponseI2C = _BV(TWEN) | _BV(TWINT) | _BV(TWEA),
    
    // 開始ビットを送信する
    	SendStartConditionBit = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA),
    
    // 停止ビットを送信する
    	SendStopConditionBit = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO),
};

/**
 * TWSRに設定する値群
 */
enum I2CStatus {
    // 開始ビットを送信した状態
    	SentStartConditionBit = 0x08,
    
    // スレーブアドレス+Wを送信し，ACKを確認した状態
    	SentSlaveAddressWithWriteBitAndACK = 0x18,
    
    // マスター動作でデータバイトを送信し，ACKを確認した状態
    	SentDataByteAndACKAtMaster = 0x28,
    
    // スレーブアドレス+Rを送信し，ACKを確認した状態
    	SentSlaveAddressWithReadBitAndACK = 0x40,
    
    // マスター動作でデータバイトを受信し，ACKを発行した状態
    	ReceivedDataByteAndACKAtMaster = 0x50,
    
    // データバイトを受信し，NACKを発行した状態
    	ReceivedDataByteAndNACK = 0x50,
    
    // 一斉呼出を受信し，ACKを発行した状態
    	ReceivedGeneralCallRecognitionAndACK = 0x70,
    
    // 自アドレス+Wを受信し，ACKを発行した状態
    	ReceivedOwnAddressWithWriteBitAndACK = 0x60,
    
    // 一斉呼出データバイトを受信し，ACKを発行した状態
    	ReceivedGeneralCallRecognitionDataByteAndACK = 0x90,
    
    // スレーブ動作でデータバイトを受信し，ACKを発行した状態
    	ReceivedDataByteAndACKAtSlave = 0x80,
    
    // 終了条件の受信した状態
    	ReceivedStopConditionBit = 0xA0,
    
    // 自アドレス+R受信した状態
    	ReceivedOwnAddressWithReadBitAndACK = 0xA8,
    
    // スレーブ動作でデータバイト送信後ACK受信した状態
    	SentDataByteAndACKAtSlave = 0xB8,
    
    // スレーブ動作で最終データバイト送信後NACKの受信した状態
    	SentDataByteAndNACKAtSlave = 0xC0
};

#endif //I2C_PRIVATE_H
