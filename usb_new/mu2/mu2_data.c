#include "mu2_common.h"
#include "mu2_data.h"

/// <summary>
/// データ送信コマンド 1
/// </summary>
/// <param name="data"></param>
/// <param name="len"></param>
/// <returns>
/// 正常終了時 : 0
/// 不正終了時 : -1
/// </returns>
int MU2_SendData(unsigned char *data, unsigned char len)
{
	char size[3];

	// Prefix
	MU2_TxChar('@');
	// Command Name
	MU2_TxChar('D');	MU2_TxChar('T');

	// Data Size
	MU2_Byte2Str(len, size);
	MU2_TxChar(size[0]);
	MU2_TxChar(size[1]);

	// Data
	while( len-- ){
		MU2_TxChar(*data);
		data++;
	}

	// Terminator
	MU2_TxChar('\r');	MU2_TxChar('\n');
	
	return 0;
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);
	if( MU2_CheckCommand(MU2_PREFIX, "DT") == 0 ){
		return 0;
	}else if( MU2_CheckCommand(MU2_PREFIX, "ER" )){
		return -1;
	}else{
		return -1;
	}
}


int MU2_ReceiveData(unsigned char *buf, unsigned char size)
{
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);

	if( MU2_CheckCommand(MU2_PREFIX, "DR") == 0){
		// RI=OFFの時
		// RI=ONの時
		unsigned char tmp[2];
		int len, i;
		tmp[0] = MU2_RX_BUF[0];
		tmp[1] = MU2_RX_BUF[1];
		len = MU2_Str2Byte((char *)tmp);

		for( i=2; (i-2)<len && (i-2)<size; i++){
			*buf = MU2_RX_BUF[i];
			buf++;
		}

		return len;
	}else if(MU2_CheckCommand(MU2_PREFIX, "ER")){
		return -1;
	}else{
		return -1;
	}
}
