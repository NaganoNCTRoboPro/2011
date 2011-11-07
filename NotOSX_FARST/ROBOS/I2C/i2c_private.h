#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

// GCRE = General Call Recognition Enable = ��Čďo
#define GCRE_BIT 	(1)
#define I2C_DOING	( ! ( TWCR & _BV(TWINT) ) )
#define I2C_STATUS	( TWSR & 0xF8 )
#define WRITE		(0)
#define READ		(1)

/**
 * TWCR�ɐݒ肷��l�Q
 */
enum I2CControl {
    // I2C�̓����������
    	EnableI2C = _BV(TWEN),
    
    // I2C�̊��荞�݂�������
    	InterruptEnableI2C = _BV(TWIE),
    
    // ACK������������
    	ACKEnableI2C = _BV(TWEA),
    
    // I2C�̓���𑱂���
    	ContinueI2C = _BV(TWEN) | _BV(TWINT),
    
    // I2C�̓���𑱂���D���C������Ԃ�
    	ContinueAndResponseI2C = _BV(TWEN) | _BV(TWINT) | _BV(TWEA),
    
    // �J�n�r�b�g�𑗐M����
    	SendStartConditionBit = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA),
    
    // ��~�r�b�g�𑗐M����
    	SendStopConditionBit = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO),
};

/**
 * TWSR�ɐݒ肷��l�Q
 */
enum I2CStatus {
    // �J�n�r�b�g�𑗐M�������
    	SentStartConditionBit = 0x08,
    
    // �X���[�u�A�h���X+W�𑗐M���CACK���m�F�������
    	SentSlaveAddressWithWriteBitAndACK = 0x18,
    
    // �}�X�^�[����Ńf�[�^�o�C�g�𑗐M���CACK���m�F�������
    	SentDataByteAndACKAtMaster = 0x28,
    
    // �X���[�u�A�h���X+R�𑗐M���CACK���m�F�������
    	SentSlaveAddressWithReadBitAndACK = 0x40,
    
    // �}�X�^�[����Ńf�[�^�o�C�g����M���CACK�𔭍s�������
    	ReceivedDataByteAndACKAtMaster = 0x50,
    
    // �f�[�^�o�C�g����M���CNACK�𔭍s�������
    	ReceivedDataByteAndNACK = 0x50,
    
    // ��Čďo����M���CACK�𔭍s�������
    	ReceivedGeneralCallRecognitionAndACK = 0x70,
    
    // ���A�h���X+W����M���CACK�𔭍s�������
    	ReceivedOwnAddressWithWriteBitAndACK = 0x60,
    
    // ��Čďo�f�[�^�o�C�g����M���CACK�𔭍s�������
    	ReceivedGeneralCallRecognitionDataByteAndACK = 0x90,
    
    // �X���[�u����Ńf�[�^�o�C�g����M���CACK�𔭍s�������
    	ReceivedDataByteAndACKAtSlave = 0x80,
    
    // �I�������̎�M�������
    	ReceivedStopConditionBit = 0xA0,
    
    // ���A�h���X+R��M�������
    	ReceivedOwnAddressWithReadBitAndACK = 0xA8,
    
    // �X���[�u����Ńf�[�^�o�C�g���M��ACK��M�������
    	SentDataByteAndACKAtSlave = 0xB8,
    
    // �X���[�u����ōŏI�f�[�^�o�C�g���M��NACK�̎�M�������
    	SentDataByteAndNACKAtSlave = 0xC0
};

#endif //I2C_PRIVATE_H
