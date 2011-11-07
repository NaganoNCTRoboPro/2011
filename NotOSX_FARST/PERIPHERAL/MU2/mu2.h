#ifndef MU2_H
#define MU2_H

#include <avr/io.h>
#include <stdbool.h>

void setMU2PutFunc(void(*f)(uint8_t c));
void setMU2GetFunc(uint8_t(*f)(void));
void byteToString(uint8_t byte, char *string);
bool mu2Command(const char *command, const char* values);
bool mu2CommandToEEPROM(const char *command, const char* values);
void mu2TransmitData(uint8_t* values, uint8_t size);

enum {
	MU2_OFFSET = 8
};

#endif //MU2_H
