#pragma once

#include "global.h"
#include "sys/stdarg.h"

#ifdef __cplusplus
extern "C"
{
#endif
    extern int printf_format(int (*push)(void *userp, unsigned char data), void *userp, const char *fmt, va_list ap);
#ifdef __cplusplus
}
#endif
