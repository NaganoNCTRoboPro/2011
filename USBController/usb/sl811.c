#include "sl811.h"
#include <led.h>

/**
 * @param {__sl811_addr} sl811アドレスアクセスポインタ
 * @param {__sl811_data} sl811データアクセスポインタ
 */
volatile Byte *__sl811_addr, *__sl811_data;

/**
 * SL811HSTの初期化
 */
void sl811_init(void)
{
	__sl811_addr = (volatile Byte *)0x8000;
	__sl811_data = (volatile Byte *)0xC000;
}

/**
 * SL811HST専用レジスタからの読み込み
 */
Byte sl811_read(const Byte offset)
{
	/**
	 * sl811からの受信データ
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
 * SL811HST専用レジスタへの書き込み
 */
void sl811_write(const Byte offset, const Byte data)
{
	*(__sl811_addr) = offset;
	_DELAY_US(150);
	*(__sl811_data) = data;
	_DELAY_US(150);
}

/**
 *  SL811HST FIFOメモリ・バッファからのブロック・データ読み込み
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
 * SL811HST FIFOメモリ・バッファへブロック・データ書き込み
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
 * パケットの送信 (USB-Aを利用)
 * @param {d} 対象デバイス
 * @param {data} 送信データ
 * @param {size} 送信データのサイズ
 * @param {flag} OUTパケット・SETUPパケットの選択
 *               SL811WRITE_OUT [0] の場合はOUTパケット
 *               SL811WRITE_SETUP [1] の場合はSETUPパケット
 * @return {size} 異常時：SL811_HANDSHAKE_NAK [-1] or SL811_HANDSHAKE_STALL [-2]
 */
SWord SL811WriteData(SL811USBDevInfo *d, Byte *data, SWord size, SWord flag)
{
	/**
	 * パケットの状態変数
	 * @type static volatile Byte
	 */
	static volatile Byte pktstatus;

	/**
	 * 未処理のデータ数 
	 * 送信データ数
	 * ループ変数
	 * ループ回数
	 *
	 * @type SWord
	 */
	SWord remain, wsiz, timeovr, retry;
	
	// 未処理のデータ数
	remain = size;

	retry = d->retry;
	d->wr_cmd |= d->pre;
	
	// USB FIFOメモリバッファの先頭アドレスの指定
	sl811_write(SL811H_HOSTBASEADDR_A, SL811H_MEMBUF_BGN);

	// 対象となるUSBデバイスのアドレスの指定
	sl811_write(SL811H_HOSTDEVADDR_A, d->addr);

	// パケット種別(OUT/SETUP)エンドポイント番号の指定
	sl811_write(SL811H_PIDDEVENDPT_A, (flag == SL811WRITE_SETUP) ? PID_SETUP : (PID_OUT | d->ep));

	do{
		// 1つのパケットで送信するデータ数を算出
		wsiz = (remain > d->epsize) ? d->epsize : remain;

		// 送信データがある場合は、USB FIFOメモリバッファへ転送する
		if( data != NULL && size > 0 )
			sl811_buf_write(SL811H_MEMBUF_BGN, &data[size - remain], wsiz);
		
		// 送信データの大きさをセット
		sl811_write(SL811H_HOSTBASELNG_A, wsiz);

		// USBデバイスがNAKの場合、再度USBデータ送信をする
		for( timeovr=0; timeovr<retry; timeovr++ )
		{

			// USB_AからUSB FIFOメモリバッファの内容を送信する
			sl811_write(SL811H_HOSTCTLREG_A, d->wr_cmd);

			_DELAY_MS(50);
			// USB_Aの送信が完了するまで待つ
			while( (sl811_read(SL811H_INTSTATUSREG) & SL811H_INTMASK_XFERDONE_A) == 0 );

			// USB_Aのパケット送信状態を読み込む
			pktstatus = sl811_read(SL811H_USBSTATUS_A);

			// USBデバイスがSTALLの場合はエラー終了
			if( pktstatus & SL811H_STATUSMASK_STALL )
			{
				return SL811_HANDSHAKE_STALL;
			}
			// USBデバイスからACKが返ってくれば、ループを抜ける
			if( pktstatus & SL811H_STATUSMASK_ACK )	break;

			// リトライの場合は5ms待つ
			_DELAY_MS(5);			
		}

		// USBデータ送信の結果がリトライをしてもACKでない場合は、エラー終了
		if( !(pktstatus & SL811H_STATUSMASK_ACK) )
		{
			return SL811_HANDSHAKE_NAK;
		}
		
		// 処理済みのデータ数を差し引く
		remain -= wsiz;

		// データパケットのトグルを行う
		d->wr_cmd ^= TOGGLE;
	}while( remain>0 );	// 残りデータがある間、繰り返す

	return size;	// USBデータ送信したサイズを返す
}


/**
 * パケットの受信 (USB-Aを利用)
 * @param {d} 対象デバイス
 * @param {data} 受信データ
 * @param {size} 受信データのサイズ
 * @return {size} 異常時：SL811_HANDSHAKE_NAK [-1] or SL811_HANDSHAKE_STALL [-2]
 */
SWord SL811ReadData(SL811USBDevInfo *d, Byte *data, SWord size)
{
	/**
	 * パケットの状態変数
	 * @type static volatile Byte
	 */
	Byte pktstatus;

	/**
	 * 未処理のデータ数 
	 * 送信データ数
	 * ループ変数
	 * ループ回数
	 *
	 * @type SWord
	 */
	SWord	timeovr, remain, rsiz, rcnt;
	
	/*
	 * 最大ループ回数
	 * 最小ループ回数
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

	// USB FIFOメモリバッファの先頭アドレスの初期値設定
	fifo = SL811H_MEMBUF_BGN;
	
	// 未処理のデータ数
	remain = size;

	// USBデバイスがNAKのときのリトライ回数
	maxretry = d->retry;

	// 短い待ち時間でのリトライ回数
	minretry = (maxretry <= 7) ? maxretry : maxretry / 3;

	d->rd_cmd |= d->pre;

	// 対象となるUSBデバイスアドレスの指定
	sl811_write(SL811H_HOSTDEVADDR_B, d->addr);

	// パケット種別(IN)とエンドポイント番号の指定
	sl811_write(SL811H_PIDDEVENDPT_B, PID_IN | d->ep);

	do{
		// 1つのパケットで送信するデータ数を算出
		rsiz = (remain > d->epsize) ? d->epsize : remain;

		// 現在のUSB FIFOメモリバッファがSL811H_MEMBUF_END(FFH)を超える場合、初期値設定
		if( (fifo + rsiz) > SL811H_MEMBUF_END )
			fifo = SL811H_MEMBUF_BGN;	

		// 現在のUSB FIFOメモリバッファの先頭アドレスの指定
		sl811_write(SL811H_HOSTBASEADDR_B, fifo);
		
		// 受信データの大きさをセット
		sl811_write(SL811H_HOSTBASELNG_B, rsiz);

		// USBデバイスがNAKの場合、再度USBデータ受信をする
		for( timeovr=0; timeovr<maxretry; timeovr++ )
		{
			

			// USB_BよりUSB FIFOメモリバッファへ内容を受信する
			sl811_write(SL811H_HOSTCTLREG_B, d->rd_cmd);

			// 200us または 2ms の時間待ち
			_DELAY_MS(2);

			// USB_Bの受信が完了するまで待つ
			while( (sl811_read(SL811H_INTSTATUSREG) & SL811H_INTMASK_XFERDONE_B) == 0 );
			
			// USB_Bのパケット受信状態を読み込む
			pktstatus = sl811_read(SL811H_USBSTATUS_B);

			// USBデバイスがSTALLの場合はエラー終了
			if( pktstatus & SL811H_STATUSMASK_STALL )
			{
				return SL811_HANDSHAKE_STALL;
			}
			// USBデバイスからACKが返ってくれば、ループを抜ける
			if( pktstatus & SL811H_STATUSMASK_ACK )	break;
		}

		// USBデータ受信の結果がリトライをしてもACKでない場合は、エラー終了
		if( timeovr >= maxretry )
		{
			return SL811_HANDSHAKE_NAK;
		}

		// 実際に受信をしたデータ数を算出
		rcnt = rsiz - (int)sl811_read(SL811H_TRANSFERCNT);

		// 受信データがある場合は、USB FIFOメモリバッファから転送する
		if( (pktstatus & SL811H_STATUSMASK_ACK) && data != 0 && size > 0 )
			sl811_buf_read(fifo, &data[size - remain], rsiz);

		// 処理済みのデータ数を差し引く
		remain -= rcnt;

		// データパケットのトグルを行う
		d->rd_cmd ^= TOGGLE;

		// 現在のUSB FIFOメモリバッファの先頭アドレスの更新
		fifo += rcnt;
	}while( remain > 0 && rcnt == rsiz );	// 残りデータがある間、繰り返す

	return size - remain;	// USBデータ受信したサイズを返す
}

/**
 * ルートデバイスのリセット
 * @return デバイス無し: SL811_USB_NONE [0]
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

		sl811_write(SL811H_SOFCNTLOW, 0xE0);	// 下位 E0
		sl811_write(SL811H_SOFCNTHIGH, SL811H_CTLREG2MASK_HOSTMODE | 0x2E);		// SOF High 2E + HOSTMode
		sl811_write(SL811H_CTLREG1, SL811H_CTLREG1MASK_ENABLESOF | SL811H_CTLREG1MASK_FULLSPEED);

		// clear all interrupt bits
		sl811_write(SL811H_INTSTATUSREG, 0xFF);
	}
	else
	{
		// Low speed device
		ret = SL811_USB_LOW;

		sl811_write(SL811H_SOFCNTLOW, 0xE0);	// 下位 E0
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
 * SL811のリセット
 *
 * @return 正常終了: 0
 *         異常終了: -1
 */
SWord SL811USBInit(void)
{
	sl811_init();
	
	return sl811_regtest();
}


/**
 * SL811HST FIFOメモリ・バッファの動作チェック（ライト・リード）
 *
 * @return 正常終了: 0
 *         異常終了: -1
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
 * @param {d} 対象デバイス
 * @param {addr} デバイスのアドレス
 */
void SL811SetAddress(SL811USBDevInfo *d, SWord addr)
{
	d->addr = addr;
}
