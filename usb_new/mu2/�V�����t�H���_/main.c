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
    unsigned char TxDataBuf[TX_DATA_LEN*5];//uart�̑��M�p�o�b�t�@�����Ɉꎞ�I�ɑ��M����f�[�^��~����
    unsigned char TxData[TX_DATA_LEN] = {'A','S','D','F','G'};//���M�������f�[�^
    
    uart_init( UART_RE|UART_TE|UART_UDRIE, 19200);//��M����|���M����|���M�f�[�^�󊄂荞�݋��C�{�[���[�g
    uart_setbuffer(TxDataBuf,TX_DATA_LEN*5);���M�p�o�b�t�@�̐ݒ�
    
    MU2_SetRxHandler(uart_getchar);//uart��1byte��M�֐����Z�b�g
    MU2_SetTxHandler(uart_buf_putchar);//uart��1byte���M�\��֐����Z�b�g ������uart_putchar������Ɗ��荞�݂ł͂Ȃ�wait�`���ɂȂ�

    sei();//�S���荞�݋���
    
    MU2_SetChannel(CHANNEL);//�`�����l���ݒ�
    MU2_SetGroupID(GROUP);//�O���[�v�ݒ�
    
    while(1){
        MU2_SendData(TxData,TX_DATA_LEN);//�f�[�^���M�\��
    }

    return 0;
}
