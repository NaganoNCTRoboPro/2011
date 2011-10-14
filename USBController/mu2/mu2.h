#ifndef MU2_H_INCLUDED
#define MU2_H_INCLUDED

#define MU2_OFFSET 8

void Byte2Str(unsigned char num, char *str);
unsigned char mu2_command(const char *cmd, char *val);
unsigned char mu2_command_eeprom(const char *cmd, char *val);
unsigned char MU2_SendData(unsigned char *data, unsigned char len);

#endif
