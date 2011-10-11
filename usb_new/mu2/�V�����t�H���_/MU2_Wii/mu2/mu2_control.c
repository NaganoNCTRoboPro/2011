#include "mu2_common.h"
#include "mu2_control.h"

/// <summary>
/// ���[�UID �ݒ�
/// </summary>
/// <param name="id">ID</param>
/// <param name="password">�p�X���[�h</param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int MU2_SetUserID(unsigned short id, unsigned short password)
{
	char id_str[5], pass_str[5];

	if( id > 0xFFFE )	return -1;

	MU2_Word2Str(id, id_str);
	MU2_Word2Str(password, pass_str);

	MU2_Command("UI", id_str, 4, ",", 1, pass_str, 4);
return 0;
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);

	if( MU2_CheckCommand(MU2_PREFIX, "UI") == 0){
		return 0;
	}else if(MU2_CheckCommand(MU2_PREFIX, "ER")){
		return -1;
	}else{
		return -1;
	}
}

/// <summary>
/// �O���[�vID �ݒ�
/// </summary>
/// <param name="id">ID</param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int MU2_SetGroupID(unsigned char id)
{
	char id_str[3];

	MU2_Byte2Str(id, id_str);
	MU2_Command("GI", id_str, 2, "", 0, "", 0);
return 0;
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);

	if( MU2_CheckCommand(MU2_PREFIX, "GI") == 0){
		return 0;
	}else if(MU2_CheckCommand(MU2_PREFIX, "ER")){
		return -1;
	}else{
		return -1;
	}
}

/// <summary>
/// �@��ID �ݒ�
/// </summary>
/// <param name="id">ID</param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int MU2_SetEquipmentID(unsigned char id)
{
	char id_str[3];

	if( id == 0x00 )	return -1;

	MU2_Byte2Str(id, id_str);
	MU2_Command("EI", id_str, 2, "", 0, "", 0);
return 0;
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);

	if( MU2_CheckCommand(MU2_PREFIX, "EI") == 0){
		return 0;
	}else if(MU2_CheckCommand(MU2_PREFIX, "ER")){
		return -1;
	}else{
		return -1;
	}
}

/// <summary>
/// �ړI��ID �w��
/// </summary>
/// <param name="id"></param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int MU2_SetDestID(unsigned char id)
{
	char id_str[3];

	MU2_Byte2Str(id, id_str);
	MU2_Command("DI", id_str, 2, "", 0, "", 0);
return 0;
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);

	if( MU2_CheckCommand(MU2_PREFIX, "DI") == 0){
		return 0;
	}else if(MU2_CheckCommand(MU2_PREFIX, "ER")){
		return -1;
	}else{
		return -1;
	}
}

/// <summary>
/// ���p���[�g�w��
/// </summary>
/// <param name="route">���[�g���</param>
/// <param name="n">���p���[�g��</param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int MU2_SetRoute(unsigned char route[], int n)
{
	int i;
	char value[3];

	// Prefix
	MU2_TxChar('@');

	// Command Name
	MU2_TxChar('R');	MU2_TxChar('T');

	// Route Information
	for(i=0; i<n; i++){
		MU2_Byte2Str(route[i], value);
		MU2_TxChar(value[0]);
		MU2_TxChar(value[1]);
		if( i != (n-1) )	MU2_TxChar(',');
	}

	// Terminator
	MU2_TxChar('\r');	MU2_TxChar('\n');

return 0;
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);
	if( MU2_CheckCommand(MU2_PREFIX, "RT") == 0){
		return 0;
	}else if(MU2_CheckCommand(MU2_PREFIX, "ER")){
		return -1;
	}else{
		return -1;
	}
}

/// <summary>
/// �g�p�`�����l���ݒ�
/// </summary>
/// <param name="ch"></param>
/// <returns>
/// ����I���� : 0
/// �s���I���� : -1
/// </returns>
int MU2_SetChannel(unsigned char ch)
{
	char value[3];

	if( ch<7 || ch>46 )		return -1;

	MU2_Byte2Str(ch, value);
	MU2_Command("CH", value, 2, "", 0, "", 0);
return 0;
	MU2_Response(MU2_PREFIX, MU2_RX_BUF);

	if( MU2_CheckCommand(MU2_PREFIX, "CH") == 0){
		return 0;
	}else if(MU2_CheckCommand(MU2_PREFIX, "ER")){
		return -1;
	}else{
		return -1;
	}
}
