/*
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart/uart.h"
#include "mu2/mu2_common.h"
#include "mu2/mu2_control.h"
#include "mu2/mu2_data.h"

#define TX_DATA_LEN 5
#define CHANNEL 0x21
#define GROUP 0x21

int main(void)
{
    unsigned char TxDataBuf[TX_DATA_LEN*5];//uartの送信用バッファここに一時的に送信するデータを蓄える
    unsigned char TxData[TX_DATA_LEN] = {'A','S','D','F','G'};//送信したいデータ
    
    uart_init( UART_RE|UART_TE|UART_UDRIE, 19200);//受信許可|送信許可|送信データ空割り込み許可，ボーレート
    uart_setbuffer(TxDataBuf,TX_DATA_LEN*5);送信用バッファの設定
    
    MU2_SetRxHandler(uart_getchar);//uartの1byte受信関数をセット
    MU2_SetTxHandler(uart_buf_putchar);//uartの1byte送信予約関数をセット 引数にuart_putcharを入れると割り込みではなくwait形式になる

    sei();//全割り込み許可
    
    MU2_SetChannel(CHANNEL);//チャンネル設定
    MU2_SetGroupID(GROUP);//グループ設定
    
    while(1){
        MU2_SendData(TxData,TX_DATA_LEN);//データ送信予約
    }

    return 0;
}
