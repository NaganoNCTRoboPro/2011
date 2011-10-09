#ifndef WAIT_H
#define WAIT_H

void wait_ms(uint16_t t);
void wait_us(uint16_t t);

#define wait(x) wati_ms(x)

#endif
