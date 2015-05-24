#pragma once

#include "global.h"
#include "sys/stdarg.h"


#ifndef PRINTF_OPTIMIZE
#define PRINTF_OPTIMIZE
#endif


#ifdef __cplusplus
extern "C"
{
#endif
    extern int printf_format(int (*push)(void *userp, unsigned char data), void *userp, const char *fmt, va_list args);
#ifdef __cplusplus
}
#endif
