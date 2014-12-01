#include "global.h"
#include "cpu/arm/cortexm/time.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "cpu/arm/cortexm/cortexutil.h"
#include "sys/time.h"
#include "sys/util.h"

#ifndef CORTEXM_SYSTICK_INTERVAL
#define CORTEXM_SYSTICK_INTERVAL 60000000
#endif

uint32_t cortexm_systick_interval;
uint32_t cortexm_systick_scale;
uint32_t cortexm_systick_last_usec;

void __attribute__((weak)) cortexm_systick_task(uint32_t time)
{
}

extern "C" void SysTick_faulthandler()
{
    cortexm_systick_task(read_usec_timer());
}

uint32_t cortexm_set_systick_interval(uint32_t interval)
{
    uint32_t ctrl = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
    uint32_t freq = cortexm_get_systick_frequency();
    if (freq < 1000000)
    {
        ctrl |= SysTick_CTRL_CLKSOURCE_Msk;
        freq = cortexm_get_fclk_frequency();
    }
    uint32_t cycles = ((uint64_t)freq) * interval / 1000000;
    if (cycles > 0x1000000) cycles = 0x1000000;
    interval = ((uint64_t)cycles) * 1000000 / freq;
    cycles = ((uint64_t)freq) * interval / 1000000;
    SysTick->CTRL = 0;
    SysTick->LOAD = cycles - 1;
    SysTick->VAL = 0;
    cortexm_systick_interval = interval;
    SysTick->CTRL = ctrl;
    cortexm_systick_scale = (((uint64_t)interval) << 28) / cycles;
    return interval;
}

#define CORTEXM_SYSTICK_CLKSOURCE_BOOT ((CORTEXM_SYSTICK_FREQUENCY_BOOT) < 1000000 ? 1 : 0)
#define CORTEXM_SYSTICK_CLKFREQ_BOOT (CORTEXM_SYSTICK_CLKSOURCE_BOOT ? (CORTEXM_FCLK_FREQUENCY_BOOT) : (CORTEXM_SYSTICK_FREQUENCY_BOOT))
#define CORTEXM_SYSTICK_CYCLES_BOOT (((uint64_t)CORTEXM_SYSTICK_CLKFREQ_BOOT) * (CORTEXM_SYSTICK_INTERVAL) / 1000000)
#define CORTEXM_SYSTICK_INTERVAL_BOOT (((uint64_t)MIN(CORTEXM_SYSTICK_CYCLES_BOOT, 0x1000000)) * 1000000 / CORTEXM_SYSTICK_CLKFREQ_BOOT)
#define CORTEXM_SYSTICK_ACTUAL_CYCLES_BOOT (((uint64_t)CORTEXM_SYSTICK_CLKFREQ_BOOT) * CORTEXM_SYSTICK_INTERVAL_BOOT / 1000000)

void time_init()
{
    SysTick->CTRL = 0;
    SysTick->LOAD = CORTEXM_SYSTICK_ACTUAL_CYCLES_BOOT - 1;
    SysTick->VAL = 0;
    cortexm_systick_interval = CORTEXM_SYSTICK_INTERVAL_BOOT;
    SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk | (SysTick_CTRL_CLKSOURCE_Msk * CORTEXM_SYSTICK_CLKSOURCE_BOOT);
    cortexm_systick_scale = (((uint64_t)CORTEXM_SYSTICK_INTERVAL_BOOT) << 32) / CORTEXM_SYSTICK_ACTUAL_CYCLES_BOOT - 1;
}

unsigned int __attribute__((optimize("-Os"))) read_usec_timer()
{
    uint32_t max = SysTick->LOAD;
    enter_critical_section();
    uint32_t now = SysTick->VAL;
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    {
        cortexm_systick_last_usec += cortexm_systick_interval;
        now = SysTick->VAL;
    }
    uint32_t last = cortexm_systick_last_usec;
    leave_critical_section();
    return last + (((1ull + cortexm_systick_scale) * (max - now)) >> 32);
}
