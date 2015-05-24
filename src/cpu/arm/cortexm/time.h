#pragma once

#include "global.h"

#ifndef CORTEX_SYSTICK_OPTIMIZE
#define CORTEX_SYSTICK_OPTIMIZE __attribute__((optimize("-Os")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
extern int cortexm_get_fclk_frequency();
extern int cortexm_get_systick_frequency();
extern void cortexm_systick_task(uint32_t time);
extern uint32_t cortexm_set_systick_interval(uint32_t interval);
extern void time_init();
#ifdef __cplusplus
}
#endif
