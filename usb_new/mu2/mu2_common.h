#ifndef __MU2_H__
#define __MU2_H__

extern void MU2_SetTxHandler(int (*fp)(unsigned char));
extern void MU2_SetRxHandler(int (*fp)(unsigned char *));
extern void MU2_SetModeHandler(void (*fp)(int));
extern void MU2_SetRtsHandler(int (*fp)(void));
extern void MU2_SetCtsHandler(void (*fp)(int));


extern int (*__mu2_tx_char)(unsigned char);
extern int (*__mu2_rx_char)(unsigned char *);
extern void (*__mu2_mode)(int);
extern int (*__mu2_rts)(void);
extern void (*__mu2_cts)(int);
extern int MU2_TxChar(unsigned char c);
extern int MU2_RxChar(unsigned char *c);

extern void MU2_Byte2Str(unsigned char num, char *str);
extern unsigned char MU2_Str2Byte(char *str);
extern void MU2_Word2Str(unsigned short num, char *str);
extern unsigned short MU2_Str2Word(char *str);
extern int MU2_CheckCommand(char *prefix1, char *prefix2);
extern int MU2_Command(
	char *cmd,
	char *value1,
	unsigned char value1_len,
	char *value2,
	unsigned char value2_len,
	char *value3,
	unsigned char value3_len);
extern int MU2_Response(char *prefix, char *value);

#define MU2_RX_BUF_SIZE	64

extern char __mu2_rx_buf[MU2_RX_BUF_SIZE];
extern char __mu2_prefix[3];
#define MU2_RX_BUF __mu2_rx_buf
#define MU2_PREFIX __mu2_prefix

#endif
