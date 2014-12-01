#pragma once

#include "global.h"

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

int fault_handler();

#ifdef __cplusplus
}
#endif

