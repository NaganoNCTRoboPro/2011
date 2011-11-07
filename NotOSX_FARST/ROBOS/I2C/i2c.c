#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "i2c.h"
#include "i2c_private.h"

#if MASTER_COMPILE

/**
 * TWI���}�X�^�[�Ƃ��ď���������
 * @example
 *     initI2CMaster(100); //100kbps�Ń}�X�^�[����ɏ�����
 *
 * @param {speed:uint16_t} I2C�̃o�X�X�s�[�h
 */
void initI2CMaster(uint16_t speed) 
{
	// �{�[���[�g��ݒ肵�CTWI�����������
		TWBR = ( ( ( FREQ * 1000 ) / speed ) - 16 ) / 2;
		TWSR = 0;
		TWCR = EnableI2C;
}

/**
 * �}�X�^�[���X���[�u��write����
 * @example
 *     uint8_t transBuf = 0, recvBuf = 0;
 *     Slave slave = { 0x01, { &transBuf, 1 }, { &recvBuf, 1 } };
 *     transBuf = 0xff;
 *     i2cWrite(&slave);   // �A�h���X��0x01�̃X���[�u��0xff���������
 *
 * @param {slave:Slave*} ���M��̃X���[�u�C���X�^���X�̃|�C���^
 */
bool i2cWrite(Slave* slave)
{
		uint8_t i, size;
	// �J�n�����̐���
		TWCR = SendStartConditionBit;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentStartConditionBit ) goto ERROR;
		
	// �A�h���X�]���CACK�m�F		
		TWDR = ( slave->address << 1 ) | WRITE;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentSlaveAddressWithWriteBitAndACK ) goto ERROR;
		
	// �f�[�^�o�C�g�]���CACK�m�F�̈�A���f�[�^�T�C�Y���s��
		for ( i = 0, size = slave->write.size; i < size; i++ )
			{
				TWDR = slave->write.buffer[i];
				TWCR = ContinueI2C;
				while ( I2C_DOING );
				if ( I2C_STATUS != SentDataByteAndACKAtMaster ) goto ERROR;
			}
		
	// �I�������̐���
		TWCR = SendStopConditionBit;
		
	// ����I��
		return false;
		
	// �G���[�P�[�X
ERROR:
		TWCR = SendStopConditionBit;
		return true;
}

/**
 * �}�X�^�[���X���[�u��read����
 * @example
 *     uint8_t transBuf = 0, recvBuf = 0;
 *     Slave slave = { 0x01, { &transBuf, 1 }, { &recvBuf, 1 } };
 *     i2cRead(&slave);   // �A�h���X��0x01�̃X���[�u����l��1byte�ǂ�
 *
 * @param {slave:Slave*} ��M��̃X���[�u�C���X�^���X�̃|�C���^
 */
bool i2cRead(Slave* slave)
{
		uint8_t i, size;
	// �J�n�����̐���
		TWCR = SendStartConditionBit;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentStartConditionBit ) goto ERROR;
		
	// �A�h���X�]���CACK�m�F
		TWDR = ( slave->address << 1 ) | READ;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentSlaveAddressWithReadBitAndACK ) goto ERROR;
		
	// �f�[�^��M�CACK���s
		for ( i = 0, size = slave->read.size - 1; i < size; i++ )
			{
				TWCR = ContinueAndResponseI2C;
				while( I2C_DOING );
				if ( I2C_STATUS != ReceivedDataByteAndACKAtMaster ) goto ERROR;
				slave->read.buffer[i] = TWDR;
			}
			
	// �ŏI�f�[�^��M�CNACK���s
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != ReceivedDataByteAndNACK ) goto ERROR;
		slave->read.buffer[size] = TWDR;
		
	// �I�������̐���
		TWCR = SendStopConditionBit;
		
	// ����I��
		return false;
		
	// �G���[�P�[�X
ERROR:
		TWCR = SendStopConditionBit;
		return true;
}

/**
 * �}�X�^�[���X���[�u�ɃR�}���h�𑗂��Ă���read����
 * @example
 *     uint8_t transBuf = 0, recvBuf = 0;
 *     Slave slave = { 0x01, { &transBuf, 1 }, { &recvBuf, 1 } };
 *     i2cReadWithCommand(&slave,cmd);   // �A�h���X��0x01�̃X���[�u��cmd�𑗂�C�Â��Ēl��1byte�ǂ�
 *
 * @param {slave:Slave*} ����M��̃X���[�u�C���X�^���X�̃|�C���^
 * @param {command:uint8_t} ���M�R�}���h
 */
bool i2cReadWithCommand(Slave* slave, uint8_t command)
{
		uint8_t i, size;
	// �J�n�����̐���
		TWCR = SendStartConditionBit;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentStartConditionBit ) goto ERROR;
    
	// �A�h���X�]��
		TWDR = ( slave->address << 1 ) | READ;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentSlaveAddressWithReadBitAndACK ) goto ERROR;
    
	// �R�}���h�]���CACK�m�F
		TWDR = command;
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != SentDataByteAndACKAtMaster ) goto ERROR;
    
	// �f�[�^�o�C�g��M�CACK���s�̈�A���f�[�^�T�C�Y-1���s��
		for ( i = 0, size = slave->read.size - 1; i < size; i++ )
		{
		    TWCR = ContinueAndResponseI2C;
		    while( I2C_DOING );
		    if ( I2C_STATUS != ReceivedDataByteAndACKAtMaster ) goto ERROR;
		    slave->read.buffer[i] = TWDR;
		}
    
	// �ŏI�f�[�^�o�C�g��M�CNACK���s
		TWCR = ContinueI2C;
		while ( I2C_DOING );
		if ( I2C_STATUS != ReceivedDataByteAndNACK ) goto ERROR;
		slave->read.buffer[size] = TWDR;
    
	// �I�������̐���
		TWCR = SendStopConditionBit;
    
	// ����I��
		return false;
    
	// �G���[�P�[�X
ERROR:
		TWCR = SendStopConditionBit;
		return true;
}
#endif // MASTER_COMPILE


#if SLAVE_COMPILE

/**
 * �X���[�u�Ŋ��荞�ݎ��Ɏg���O���[�o���ϐ��̐錾
 */
static volatile Slave* own;
static volatile uint8_t byteCount;
static volatile bool emergency = false;
static volatile bool i2cComFlag = false;	

/**
 * TWI���X���[�u�Ƃ��ď���������
 * @example
 *     uint8_t recvBuf = 0, transBuf = 0;
 *     //�}�X�^�[���_��write�Cread�Ȃ̂ŃX���[�u����write�͎�M�Cread�͑��M�����ɂȂ�
 *     Slave own = { 0x01, { &recvBuf, 1 }, { &transBuf, 1 } };
 *     initI2CSlave(&own);   // �A�h���X��0x01�̃X���[�u�Ɏ��g���Z�b�g�����
 *     sei();
 *
 * @param {_own:Slave*} �������g�̃X���[�u�C���X�^���X�̃|�C���^
 */
void initI2CSlave(Slave* _own) 
{
	// ���A�h���X��ݒ肵�CTWI�CACK�C���荞�݁C��Čďo����������
    own = _own;
    TWAR = own->address << 1;
    TWAR |= GCRE_BIT;		
    TWCR = ACKEnableI2C | EnableI2C | InterruptEnableI2C;
}

/**
 * ��펖�Ԍ����֐�
 * @return {bool} ��펞���ۂ�
 */

bool isEmergency(void)
{
	return emergency;
}


/**
 * �ʐM�������֐�
 * @return {bool} �ʐM�����ۂ�
 */
bool isCommunicatingViaI2C(void)
{
	return i2cComFlag;
}


/**
 * �X���[�u����̎��̊��荞�ݏ���
 */
ISR (TWI_vect)
{
    switch (TWSR)
        {
            // ��Čďo��M�C��펞�ƔF��Cbreak�Ȃ�
                case ReceivedGeneralCallRecognitionAndACK:
                    emergency = true;
                
            // ���A�h���X+W��M�CTWINT�����낷
                case ReceivedOwnAddressWithWriteBitAndACK:
                    byteCount = 0;
                    i2cComFlag = true;
                    TWCR |= ContinueI2C;
                    break;
                
            // ��Čďo�f�[�^�o�C�g��M�C�T�|�[�g���Ȃ�
                case ReceivedGeneralCallRecognitionDataByteAndACK:
                    TWCR |= ContinueI2C;
                    break;
                
            // �f�[�^�o�C�g��M�C�o�b�t�@�Ɉڂ���TWINT�����낷
                case ReceivedDataByteAndACKAtSlave:
                    own->write.buffer[byteCount++] = TWDR;
                    break;
                
            // �I�������̎�M�CTWINT�����낷
                case ReceivedStopConditionBit:
                    TWCR |= ContinueI2C;
                    break;
                
            // ���A�h���X+R��M�C1�o�C�g�ڂ̃f�[�^�o�C�g�̑��M���s���CTWINT�����낷
                case ReceivedOwnAddressWithReadBitAndACK:
                    byteCount = 0;
                    i2cComFlag = true;
                    TWDR = own->read.buffer[byteCount++];				
                    TWCR |= ContinueI2C;
                    break;
                
            // ACK��M�C�f�[�^�o�C�g�̑��M���s���CTWINT�����낷
                case SentDataByteAndACKAtSlave:
                    TWDR = own->read.buffer[byteCount++];
                    TWCR |= ContinueI2C;
                    break;
                
            // NACK�̎�M�CTWINT�����낷
                case SentDataByteAndNACKAtSlave:
                    TWCR |= ContinueI2C;
                    i2cComFlag = false;
                    break;
                
            // ���CTWCR�̏�����
                default:
                    TWCR = ACKEnableI2C | EnableI2C | InterruptEnableI2C;
                    i2cComFlag = false;
                    break;
        }
}
#endif // SLAVE_COMPILE
