#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif

void set_printf_output(int (*fp)(char));
int __printf(const char *format, ...) __attribute__((format(printf, 1, 2)));

#define printf(format, args...)	__printf(format,## args)

#ifdef __cplusplus
}
#endif

#endif
