#include "global.h"
#define NO_INLINE_VERSION
#include "cpu/arm/cortexm/cortexutil.h"
#include "sys/util.h"

void __attribute__((optimize("-Os"))) idle()
{
    __asm__ volatile("wfi");
}

void __attribute__((optimize("-Os"))) enter_critical_section()
{
    __asm__ volatile("cpsid if");
}

void __attribute__((optimize("-Os"))) leave_critical_section()
{
    __asm__ volatile("cpsie if");
}

__attribute__((const,optimize("-Os"))) uint32_t swap32(uint32_t data)
{
    __asm__("rev %[data], %[data]" : [data] "+r" (data));
    return data;
}

__attribute__((const,optimize("-Os"))) uint32_t swap16(uint32_t data)
{
    __asm__("rev16 %[data], %[data]" : [data] "+r" (data));
    return data;
}

#ifndef CPU_ARM_CORTEX_M0
__attribute__((const,optimize("-Os"))) uint32_t reverse32(uint32_t data)
{
    __asm__("rbit %[data], %[data]" : [data] "+r" (data));
    return data;
}
#endif

extern "C" __attribute__((weak)) uint32_t __aeabi_idiv0()
{
    return (uint32_t)fault_handler();
}

extern "C" __attribute__((weak)) uint64_t __aeabi_ldiv0()
{
    return (uint64_t)__aeabi_ldiv0();
}
