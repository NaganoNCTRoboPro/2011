#include <avr/io.h>
#include "mu2.h"
#include <uart.h>


static uint8_t i,tmp[10];
static char ascii[16] = "0123456789ABCDEF";

/// <summary>
/// �o�C�g�i0x00�`0xFF�j�̐���2���̕�����ɕϊ�
/// </summary>
/// <param name="num">�ϊ��Ώۂ̐���</param>
/// <param name="str">�ϊ���̕�������i�[����̈�</param>
void Byte2Str(unsigned char num, char *str)
{
	str[0] = ascii[(num >> 4) & 0x0F];
	str[1] = ascii[num&0x0F];
	str[2] = '\0';
}

unsigned char mu2_command(const char *cmd, char *val){
	uart0_putchar('@');
	uart0_putchar(cmd[0]);
	uart0_putchar(cmd[1]);
	while(*val!='\0'){
		uart0_putchar(*val);
		val++;
	}
	uart0_putchar(0x0d);
	uart0_putchar(0x0a);
	for(i=0;i<10;i++){
		tmp[i] = uart0_getchar();
		uart1_putchar(tmp[i]);
		if(i>0&&tmp[i-1] == 0x0d && tmp[i] == 0x0a) break;
	} 
	if(tmp[1]=='E'&&tmp[2]=='R'){return 1;}
	return 0;
}

unsigned char mu2_command_eeprom(const char *cmd, char *val){
	uart0_putchar('@');
	uart0_putchar(cmd[0]);
	uart0_putchar(cmd[1]);
	while(*val!='\0'){
		uart0_putchar(*val);
		val++;
	}
	uart0_putchar('/');
	uart0_putchar('W');
	uart0_putchar(0x0d);
	uart0_putchar(0x0a);
	for(i=0;i<10;i++){
		tmp[i] = uart0_getchar();
		uart1_putchar(tmp[i]);
		if(i>0&&tmp[i-1] == 0x0d && tmp[i] == 0x0a) break;
	} 
	if(tmp[1]=='E'&&tmp[2]=='R'){return 1;}
	return 0;
}
