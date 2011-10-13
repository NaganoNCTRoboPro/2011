#include <avr/io.h>
#include "wait.h"

/**
 * wait for ms
 *
 * @param {t} wait time
 */
void wait_ms(uint16_t t){
	while(t--){
		wait_us(999);
	}
	return;
}

/**
 * wait for us
 *
 * @param {t} wait time
 */
void wait_us(uint16_t t){
  unsigned char lpcnt=0;
  __asm__ __volatile__("\n"
    "CPU_wait_entry%=:\n\t"
    "ldi %0,1\n"
    "CPU_wait_lp%=:\n\t"
    "nop\n\t"
    "dec %0\n\t"
    "brne CPU_wait_lp%=\n\t"
    "sbiw %1,1\n\t"
    "brne CPU_wait_entry%=\n\t"
    :"=&a"(lpcnt)
    :"w"(t)
  );
  return;
}


