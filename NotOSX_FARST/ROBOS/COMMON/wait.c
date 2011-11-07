#include <avr/io.h>
void wait_us(uint16_t t){
  static unsigned char lpcnt;
  __asm__ __volatile__("\n"
    "CPU_wait_entry%=:\n\t"
    "ldi %0,4\n"
    "CPU_wait_lp%=:\n\t"
    "nop\n\t"
    "dec %0\n\t"
    "brne CPU_wait_lp%=\n\t"
    "sbiw %1,1\n\t"
    "brne CPU_wait_entry%=\n\t"
    :"=&a"(lpcnt)
    :"w"(t)
  );
}
void wait_ms(uint16_t t){
	while(t--) wait_us(1000);
	
}

