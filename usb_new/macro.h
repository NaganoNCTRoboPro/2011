#ifndef MACRO
#define MACRO

typedef uint16_t word;
typedef uint8_t byte;

#define putchar uart0_putchar

#define DBG printf

#define NULL ((void *)0)

#define SWITCH	((PINF^0xff) & 0xF0)
//#define SELECTER ((PINF^0xff) & 0x01)

#define F_CPU 8000000UL
#define DELAY _delay_us(150)
#define DELAY_LED _delay_ms(500)
#define DELAY_USB _delay_ms(250)

#define LED0_ON PORTD |= _BV(PORTD2)
#define LED1_ON PORTD |= _BV(PORTD4)
#define LED2_ON PORTD |= _BV(PORTD6)
#define LED_ON_ALL PORTD |= _BV(PORTD2) | _BV(PORTD4) | _BV(PORTD6)
#define LED0_OFF PORTD &= ~_BV(PORTD2)
#define LED1_OFF PORTD &= ~_BV(PORTD4)
#define LED2_OFF PORTD &= ~_BV(PORTD6)
#define LED_OFF_ALL PORTD &= ~(_BV(PORTD2) | _BV(PORTD4) | _BV(PORTD6))

#define VBUS_ON PORTD &= ~_BV(PORTD7)
#define VBUS_OFF PORTD |= _BV(PORTD7)

/*#define SW0x0 PINF==0xFF	//0000 0-->OFF
#define SW0x1 PINF==0x7F	//1000 1-->ON
#define SW0x2 PINF==0xBF	//0100
#define SW0x3 PINF==0x3F	//1100
#define SW0x4 PINF==0xDF	//0010
#define SW0x5 PINF==0x5F	//1010
#define SW0x6 PINF==0x9F	//0110
#define SW0x7 PINF==0x1F	//1110
#define SW0x8 PINF==0xEF	//0001
#define SW0x9 PINF==0x6F	//1001
#define SW0xA PINF==0xAF	//0101
#define SW0xB PINF==0x2F	//1101
#define SW0xC PINF==0xCF	//0011
#define SW0xD PINF==0x4F	//1011
#define SW0xE PINF==0x8F	//0111
#define SW0xF PINF==0x0F	//1111*/

#endif
