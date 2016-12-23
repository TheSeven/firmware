#pragma once

#include "global.h"

#ifndef CORTEXUTIL_OPTIMIZE
#define CORTEXUTIL_OPTIMIZE __attribute__((optimize("-Os")))
#endif

#if defined(CPU_ARM_CORTEX_M3) || defined(CPU_ARM_CORTEX_M4)
#define BB_SRAM(addr, bit) (*(uint32_t*)(0x22000000 + ((((uintptr_t)&(addr)) - 0x20000000) * 32) + (bit) * 4))
#define BB_PERIPH(addr, bit) (*(uint32_t*)(0x42000000 + ((((uintptr_t)&(addr)) - 0x40000000) * 32) + (bit) * 4))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

static inline void WFI()
{
    __asm__ volatile("wfi" ::: "memory");
}

static inline void WFE()
{
    __asm__ volatile("wfe" ::: "memory");
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

