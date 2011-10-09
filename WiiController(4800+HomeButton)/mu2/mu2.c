#include "mu2.h"

int (*__mu2_tx_char)(unsigned char);
int (*__mu2_rx_char)(unsigned char *);

char __mu2_rx_buf[MU2_RX_BUF_SIZE];
char __mu2_prefix[3];

static unsigned char phase = 0;
static          char rx_buf[MU2_RX_BUF_SIZE];
static unsigned char rx_count;
static unsigned char i;

__inline__ void MU2_SetTxHandler(int (*fp)(unsigned char)){__mu2_tx_char=fp;}
__inline__ void MU2_SetRxHandler(int (*fp)(unsigned char *)){__mu2_rx_char=fp;}
__inline__ int MU2_TxChar(unsigned char c){ return (*__mu2_tx_char)(c);	}
__inline__ int MU2_RxChar(unsigned char *c){ return (*__mu2_rx_char)(c); }


static __inline__ void Byte2Str(unsigned char num, char *str)
{
	int tmp;

	tmp = (num >> 4) & 0x0F;
	str[0] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	tmp = num & 0x0F;
	str[1] = tmp + ((tmp > 9) ? ('A'-10) : '0');
	str[2] = '\0';
}

static __inline__ unsigned char Str2Byte(char *str)
{
	unsigned char tmp;

	tmp = str[0] - ((str[0] >= 'A') ? ('A'-10) : '0' );
	tmp <<= 4;
	tmp |= str[1] - ((str[1] >= 'A') ? ('A'-10) : '0' );

	return tmp;
}


int MU2_CheckCommand(char *prefix1, char *prefix2)
{
	if( (prefix1[0] == prefix2[0]) && (prefix1[1] == prefix2[1]) ){
		return 0;
	}else{
		return -1;
	}
}


int MU2_Command( char *cmd, char *value, unsigned char value_len )
{

	// Prefix
	MU2_TxChar('@');

	// Command Name
	MU2_TxChar( cmd[0] );
	MU2_TxChar( cmd[1] );

	// Value 1
	while( value_len-- )	MU2_TxChar( *value++ );

	// Terminator
	MU2_TxChar('\r');	MU2_TxChar('\n');

	return 0;
}


int MU2_Response(char rx_data,char *prefix, char *value)
{
    switch(phase){
        case 0:
            if(rx_data=='*')
                {
                    rx_count = 0;
                    phase = 1;
                }
            break;
        case 1:
            if( rx_data == '\n' ){
                if( rx_buf[rx_count-1] == '\r')
                    {
                        for(i=3;i<rx_count-1;i++,value++)
                            {
                                *value = rx_buf[i];
                            }
                        *value = '\0';
                        phase = 0;
                        return 1;
                    }
            }
            break;
    }

    rx_buf[rx_count++] = rx_data;

    return 0;
}

int MU2_SendData(unsigned char *data, unsigned char len)
{
	char size[3];

	// Prefix
	MU2_TxChar('@');
	// Command Name
	MU2_TxChar('D');	MU2_TxChar('T');

	// Data Size
	Byte2Str(len, size);
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
}


int MU2_ReceiveData(unsigned char *buf, unsigned char size)
{
    unsigned char c;
    MU2_RxChar(&c);
	if(MU2_Response(c,MU2_PREFIX, MU2_RX_BUF)){

        if( MU2_CheckCommand(MU2_PREFIX, "DR") == 0){
            // RI=OFFの時
            // RI=ONの時
            unsigned char tmp[2];
            int len, i;
            tmp[0] = MU2_RX_BUF[0];
            tmp[1] = MU2_RX_BUF[1];
            len = Str2Byte((char *)tmp);

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
	return 0;
}

int MU2_SetChannel(unsigned char ch)
{
	char value[3];

	if( ch<7 || ch>46 )		return -1;

	Byte2Str(ch, value);
	MU2_Command("CH", value, 2);

	return 0;
}

int MU2_SetGroupID(unsigned char id)
{
	char id_str[3];

	Byte2Str(id, id_str);
	MU2_Command("GI", id_str, 2);

	return 0;
}
int MU2_SetEquipmentID(unsigned char id)
{
	char id_str[3];

	Byte2Str(id, id_str);
	MU2_Command("EI", id_str, 2);

	return 0;
}
int MU2_SetDestinationID(unsigned char id)
{
	char id_str[3];

	Byte2Str(id, id_str);
	MU2_Command("DI", id_str, 2);

	return 0;
}

int MU2_SendDataBus(unsigned char *data, unsigned char len)
{
	char size[3];

	// Prefix
	MU2_TxChar('*');
	// Command Name
	MU2_TxChar('D');	MU2_TxChar('R');
	MU2_TxChar('=');

	// Data Size
	Byte2Str(len, size);
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
}

