#ifndef MU2_H_INCLUDED
#define MU2_H_INCLUDED


extern int (*__mu2_tx_char)(unsigned char);
extern int (*__mu2_rx_char)(unsigned char *);

void MU2_SetTxHandler(int (*fp)(unsigned char));
void MU2_SetRxHandler(int (*fp)(unsigned char *));

extern int MU2_TxChar(unsigned char c);
extern int MU2_RxChar(unsigned char *c);


extern int MU2_CheckCommand(char *prefix1, char *prefix2);
extern int MU2_Command( char *cmd,char *value,unsigned char value_len);

#define MU2_RX_BUF_SIZE	64

extern char __mu2_rx_buf[MU2_RX_BUF_SIZE];
extern char __mu2_prefix[3];
#define MU2_RX_BUF __mu2_rx_buf
#define MU2_PREFIX __mu2_prefix

int MU2_SendData(unsigned char *data, unsigned char len);
int MU2_Response(char rx_data,char *prefix, char *value);
int MU2_SetChannel(unsigned char ch);
int MU2_SetGroupID(unsigned char id);
int MU2_SetBoudRate(char b);
int MU2_SendDataBus(unsigned char *data, unsigned char len);

#endif // MU2_H_INCLUDED
