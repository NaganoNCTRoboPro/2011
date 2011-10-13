#include "sl811.h"
#include <led.h>

/**
 * @param {__sl811_addr} sl811�A�h���X�A�N�Z�X�|�C���^
 * @param {__sl811_data} sl811�f�[�^�A�N�Z�X�|�C���^
 */
volatile Byte *__sl811_addr, *__sl811_data;

/**
 * SL811HST�̏�����
 */
void sl811_init(void)
{
	__sl811_addr = (volatile Byte *)0x8000;
	__sl811_data = (volatile Byte *)0xC000;
}

/**
 * SL811HST��p���W�X�^����̓ǂݍ���
 */
Byte sl811_read(const Byte offset)
{
	/**
	 * sl811����̎�M�f�[�^
	 * @type Byte
	 */
	Byte data;

	*(__sl811_addr) = offset;
	_DELAY_US(150);
	data = *(__sl811_data);
	_DELAY_US(150);

	return data;
}

/**
 * SL811HST��p���W�X�^�ւ̏�������
 */
void sl811_write(const Byte offset, const Byte data)
{
	*(__sl811_addr) = offset;
	_DELAY_US(150);
	*(__sl811_data) = data;
	_DELAY_US(150);
}

/**
 *  SL811HST FIFO�������E�o�b�t�@����̃u���b�N�E�f�[�^�ǂݍ���
 */
void sl811_buf_read(const Byte offset, Byte *buf, SWord size)
{
	*(__sl811_addr) = offset;
	_DELAY_US(150);

	while( size-- ){
		 *buf = *(__sl811_data);
		 _DELAY_US(150);
		 buf++;
	}
}

/**
 * SL811HST FIFO�������E�o�b�t�@�փu���b�N�E�f�[�^��������
 */
void sl811_buf_write(const Byte offset, const Byte *buf, SWord size)
{
	*(__sl811_addr) = offset;
	_DELAY_US(150);

	while( size-- ){
		*(__sl811_data) = *buf;
		_DELAY_US(150);
		buf++;
	}
}

/**
 * �p�P�b�g�̑��M (USB-A�𗘗p)
 * @param {d} �Ώۃf�o�C�X
 * @param {data} ���M�f�[�^
 * @param {size} ���M�f�[�^�̃T�C�Y
 * @param {flag} OUT�p�P�b�g�ESETUP�p�P�b�g�̑I��
 *               SL811WRITE_OUT [0] �̏ꍇ��OUT�p�P�b�g
 *               SL811WRITE_SETUP [1] �̏ꍇ��SETUP�p�P�b�g
 * @return {size} �ُ펞�FSL811_HANDSHAKE_NAK [-1] or SL811_HANDSHAKE_STALL [-2]
 */
SWord SL811WriteData(SL811USBDevInfo *d, Byte *data, SWord size, SWord flag)
{
	/**
	 * �p�P�b�g�̏�ԕϐ�
	 * @type static volatile Byte
	 */
	static volatile Byte pktstatus;

	/**
	 * �������̃f�[�^�� 
	 * ���M�f�[�^��
	 * ���[�v�ϐ�
	 * ���[�v��
	 *
	 * @type SWord
	 */
	SWord remain, wsiz, timeovr, retry;
	
	// �������̃f�[�^��
	remain = size;

	retry = d->retry;
	d->wr_cmd |= d->pre;
	
	// USB FIFO�������o�b�t�@�̐擪�A�h���X�̎w��
	sl811_write(SL811H_HOSTBASEADDR_A, SL811H_MEMBUF_BGN);

	// �ΏۂƂȂ�USB�f�o�C�X�̃A�h���X�̎w��
	sl811_write(SL811H_HOSTDEVADDR_A, d->addr);

	// �p�P�b�g���(OUT/SETUP)�G���h�|�C���g�ԍ��̎w��
	sl811_write(SL811H_PIDDEVENDPT_A, (flag == SL811WRITE_SETUP) ? PID_SETUP : (PID_OUT | d->ep));

	do{
		// 1�̃p�P�b�g�ő��M����f�[�^�����Z�o
		wsiz = (remain > d->epsize) ? d->epsize : remain;

		// ���M�f�[�^������ꍇ�́AUSB FIFO�������o�b�t�@�֓]������
		if( data != NULL && size > 0 )
			sl811_buf_write(SL811H_MEMBUF_BGN, &data[size - remain], wsiz);
		
		// ���M�f�[�^�̑傫�����Z�b�g
		sl811_write(SL811H_HOSTBASELNG_A, wsiz);

		// USB�f�o�C�X��NAK�̏ꍇ�A�ēxUSB�f�[�^���M������
		for( timeovr=0; timeovr<retry; timeovr++ )
		{

			// USB_A����USB FIFO�������o�b�t�@�̓��e�𑗐M����
			sl811_write(SL811H_HOSTCTLREG_A, d->wr_cmd);

			_DELAY_MS(50);
			// USB_A�̑��M����������܂ő҂�
			while( (sl811_read(SL811H_INTSTATUSREG) & SL811H_INTMASK_XFERDONE_A) == 0 );

			// USB_A�̃p�P�b�g���M��Ԃ�ǂݍ���
			pktstatus = sl811_read(SL811H_USBSTATUS_A);

			// USB�f�o�C�X��STALL�̏ꍇ�̓G���[�I��
			if( pktstatus & SL811H_STATUSMASK_STALL )
			{
				return SL811_HANDSHAKE_STALL;
			}
			// USB�f�o�C�X����ACK���Ԃ��Ă���΁A���[�v�𔲂���
			if( pktstatus & SL811H_STATUSMASK_ACK )	break;

			// ���g���C�̏ꍇ��5ms�҂�
			_DELAY_MS(5);			
		}

		// USB�f�[�^���M�̌��ʂ����g���C�����Ă�ACK�łȂ��ꍇ�́A�G���[�I��
		if( !(pktstatus & SL811H_STATUSMASK_ACK) )
		{
			return SL811_HANDSHAKE_NAK;
		}
		
		// �����ς݂̃f�[�^������������
		remain -= wsiz;

		// �f�[�^�p�P�b�g�̃g�O�����s��
		d->wr_cmd ^= TOGGLE;
	}while( remain>0 );	// �c��f�[�^������ԁA�J��Ԃ�

	return size;	// USB�f�[�^���M�����T�C�Y��Ԃ�
}


/**
 * �p�P�b�g�̎�M (USB-A�𗘗p)
 * @param {d} �Ώۃf�o�C�X
 * @param {data} ��M�f�[�^
 * @param {size} ��M�f�[�^�̃T�C�Y
 * @return {size} �ُ펞�FSL811_HANDSHAKE_NAK [-1] or SL811_HANDSHAKE_STALL [-2]
 */
SWord SL811ReadData(SL811USBDevInfo *d, Byte *data, SWord size)
{
	/**
	 * �p�P�b�g�̏�ԕϐ�
	 * @type static volatile Byte
	 */
	Byte pktstatus;

	/**
	 * �������̃f�[�^�� 
	 * ���M�f�[�^��
	 * ���[�v�ϐ�
	 * ���[�v��
	 *
	 * @type SWord
	 */
	SWord	timeovr, remain, rsiz, rcnt;
	
	/*
	 * �ő僋�[�v��
	 * �ŏ����[�v��
	 *
	 * @type SWord
	 */
	SWord	maxretry, minretry;
	
	/*
	 * FIFO
	 *
	 * @type SWord 
	 */
	SWord	fifo;

	// USB FIFO�������o�b�t�@�̐擪�A�h���X�̏����l�ݒ�
	fifo = SL811H_MEMBUF_BGN;
	
	// �������̃f�[�^��
	remain = size;

	// USB�f�o�C�X��NAK�̂Ƃ��̃��g���C��
	maxretry = d->retry;

	// �Z���҂����Ԃł̃��g���C��
	minretry = (maxretry <= 7) ? maxretry : maxretry / 3;

	d->rd_cmd |= d->pre;

	// �ΏۂƂȂ�USB�f�o�C�X�A�h���X�̎w��
	sl811_write(SL811H_HOSTDEVADDR_B, d->addr);

	// �p�P�b�g���(IN)�ƃG���h�|�C���g�ԍ��̎w��
	sl811_write(SL811H_PIDDEVENDPT_B, PID_IN | d->ep);

	do{
		// 1�̃p�P�b�g�ő��M����f�[�^�����Z�o
		rsiz = (remain > d->epsize) ? d->epsize : remain;

		// ���݂�USB FIFO�������o�b�t�@��SL811H_MEMBUF_END(FFH)�𒴂���ꍇ�A�����l�ݒ�
		if( (fifo + rsiz) > SL811H_MEMBUF_END )
			fifo = SL811H_MEMBUF_BGN;	

		// ���݂�USB FIFO�������o�b�t�@�̐擪�A�h���X�̎w��
		sl811_write(SL811H_HOSTBASEADDR_B, fifo);
		
		// ��M�f�[�^�̑傫�����Z�b�g
		sl811_write(SL811H_HOSTBASELNG_B, rsiz);

		// USB�f�o�C�X��NAK�̏ꍇ�A�ēxUSB�f�[�^��M������
		for( timeovr=0; timeovr<maxretry; timeovr++ )
		{
			

			// USB_B���USB FIFO�������o�b�t�@�֓��e����M����
			sl811_write(SL811H_HOSTCTLREG_B, d->rd_cmd);

			// 200us �܂��� 2ms �̎��ԑ҂�
			_DELAY_MS(2);

			// USB_B�̎�M����������܂ő҂�
			while( (sl811_read(SL811H_INTSTATUSREG) & SL811H_INTMASK_XFERDONE_B) == 0 );
			
			// USB_B�̃p�P�b�g��M��Ԃ�ǂݍ���
			pktstatus = sl811_read(SL811H_USBSTATUS_B);

			// USB�f�o�C�X��STALL�̏ꍇ�̓G���[�I��
			if( pktstatus & SL811H_STATUSMASK_STALL )
			{
				return SL811_HANDSHAKE_STALL;
			}
			// USB�f�o�C�X����ACK���Ԃ��Ă���΁A���[�v�𔲂���
			if( pktstatus & SL811H_STATUSMASK_ACK )	break;
		}

		// USB�f�[�^��M�̌��ʂ����g���C�����Ă�ACK�łȂ��ꍇ�́A�G���[�I��
		if( timeovr >= maxretry )
		{
			return SL811_HANDSHAKE_NAK;
		}

		// ���ۂɎ�M�������f�[�^�����Z�o
		rcnt = rsiz - (int)sl811_read(SL811H_TRANSFERCNT);

		// ��M�f�[�^������ꍇ�́AUSB FIFO�������o�b�t�@����]������
		if( (pktstatus & SL811H_STATUSMASK_ACK) && data != 0 && size > 0 )
			sl811_buf_read(fifo, &data[size - remain], rsiz);

		// �����ς݂̃f�[�^������������
		remain -= rcnt;

		// �f�[�^�p�P�b�g�̃g�O�����s��
		d->rd_cmd ^= TOGGLE;

		// ���݂�USB FIFO�������o�b�t�@�̐擪�A�h���X�̍X�V
		fifo += rcnt;
	}while( remain > 0 && rcnt == rsiz );	// �c��f�[�^������ԁA�J��Ԃ�

	return size - remain;	// USB�f�[�^��M�����T�C�Y��Ԃ�
}

/**
 * ���[�g�f�o�C�X�̃��Z�b�g
 * @return �f�o�C�X����: SL811_USB_NONE [0]
 *         LowSpeed:     SL811_USB_LOW  [1]
 *         FullSpeed:    SL811_USB_FULL [2]
 */
SWord SL811USBReset(void)
{
	SWord ret;
	Byte status;

	_DELAY_MS(100);

	// Config host mode
	sl811_write(SL811H_CTLREG2, SL811H_CTLREG2MASK_HOSTMODE);

	sl811_write(SL811H_CTLREG1, SL811H_CTLREG1VAL_RESET);	// reset USB
	_DELAY_MS(30);
	sl811_write(SL811H_CTLREG1, 0x00);						// remove SE0

	for(status=0; status<100; status++)
		sl811_write(SL811H_INTSTATUSREG, 0xFF);				// clear all interrupt bits

	// Full/Low Speed Detection
	status = sl811_read(SL811H_INTSTATUSREG);
	
	if( status & SL811H_INTMASK_NODEVICE )
	{
		// USB device none

		sl811_write(SL811H_INTENBLREG,
			SL811H_INTMASK_SOFINTR 		// Interrupt on SOF Timer
		);
		return SL811_USB_NONE;
	}

	if( status & SL811H_INTMASK_DATAPLUS )
	{
		// Full speed device
		ret = SL811_USB_FULL;

		sl811_write(SL811H_SOFCNTLOW, 0xE0);	// ���� E0
		sl811_write(SL811H_SOFCNTHIGH, SL811H_CTLREG2MASK_HOSTMODE | 0x2E);		// SOF High 2E + HOSTMode
		sl811_write(SL811H_CTLREG1, SL811H_CTLREG1MASK_ENABLESOF | SL811H_CTLREG1MASK_FULLSPEED);

		// clear all interrupt bits
		sl811_write(SL811H_INTSTATUSREG, 0xFF);
	}
	else
	{
		// Low speed device
		ret = SL811_USB_LOW;

		sl811_write(SL811H_SOFCNTLOW, 0xE0);	// ���� E0
		sl811_write(SL811H_SOFCNTHIGH, SL811H_CTLREG2MASK_HOSTMODE | SL811H_CTLREG2MASK_DSWAP | 0x2E);	// SOF High 2E + HOSTMode + Data Priority Swap
		sl811_write(SL811H_CTLREG1, SL811H_CTLREG1MASK_ENABLESOF | SL811H_CTLREG1MASK_LOWSPEED);
		
		// clear all interrupt bits
		for(status=0; status<20; status++)
			sl811_write(SL811H_INTSTATUSREG, 0xFF);
	}

	// enable all interrupts
	sl811_write(SL811H_INTENBLREG,
		SL811H_INTMASK_SOFINTR		// Interrupt on SOF Timer
	);
	
	return ret;
}

/**
 * SL811�̃��Z�b�g
 *
 * @return ����I��: 0
 *         �ُ�I��: -1
 */
SWord SL811USBInit(void)
{
	sl811_init();
	
	return sl811_regtest();
}


/**
 * SL811HST FIFO�������E�o�b�t�@�̓���`�F�b�N�i���C�g�E���[�h�j
 *
 * @return ����I��: 0
 *         �ُ�I��: -1
 */
SWord sl811_regtest(void)
{
	int	i, data;

	for( i=SL811H_MEMBUF_BGN; i<SL811H_MEMBUF_END; i++ )
	{
		sl811_write(i, 256-i);
		data = sl811_read(i);
		if( data != (256-i) )
		{
			return -1;
		}
	}
	
	return 0;
}



/**
 * SL811 Set Address
 *
 * @param {d} �Ώۃf�o�C�X
 * @param {addr} �f�o�C�X�̃A�h���X
 */
void SL811SetAddress(SL811USBDevInfo *d, SWord addr)
{
	d->addr = addr;
}
