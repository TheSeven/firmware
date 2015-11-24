#pragma once

#include "global.h"

#ifndef CORTEXUTIL_OPTIMIZE
#define CORTEXUTIL_OPTIMIZE __attribute__((optimize("-Os")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

static inline void WFI()
{
    __asm__ volatile("wfi");
}

static inline void WFE()
{
    __asm__ volatile("wfe");
}

static inline void SEV()
{
    __asm__ volatile("sev");
}

static inline unsigned int read_ipsr()
{
    unsigned int result;
    __asm__ volatile("mrs %[result], ipsr" : [result] "=r" (result));
    return result;
}

void fault_handler();

#ifdef __cplusplus
}
#endif

