#include "global.h"
#define NO_INLINE_VERSION
#include "cpu/arm/cortexm/cortexutil.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "sys/util.h"

void CORTEXUTIL_OPTIMIZE idle()
{
    __asm__ volatile("wfi");
}

void CORTEXUTIL_OPTIMIZE reset()
{
    NVIC_SystemReset();
}

void CORTEXUTIL_OPTIMIZE enter_critical_section()
{
    __asm__ volatile("cpsid if");
}

void CORTEXUTIL_OPTIMIZE leave_critical_section()
{
    __asm__ volatile("cpsie if");
}

uint32_t __attribute__((const)) CORTEXUTIL_OPTIMIZE swap32(uint32_t data)
{
    __asm__("rev %[data], %[data]" : [data] "+r" (data));
    return data;
}

uint32_t __attribute__((const)) CORTEXUTIL_OPTIMIZE swap16(uint32_t data)
{
    __asm__("rev16 %[data], %[data]" : [data] "+r" (data));
    return data;
}

#ifndef CPU_ARM_CORTEX_M0
uint32_t __attribute__((const)) CORTEXUTIL_OPTIMIZE reverse32(uint32_t data)
{
    __asm__("rbit %[data], %[data]" : [data] "+r" (data));
    return data;
}
#endif

extern "C" __attribute__((weak)) CORTEXUTIL_OPTIMIZE uint32_t __aeabi_idiv0()
{
    return (uint32_t)fault_handler();
}

extern "C" __attribute__((weak)) CORTEXUTIL_OPTIMIZE uint64_t __aeabi_ldiv0()
{
    return (uint64_t)__aeabi_ldiv0();
}
