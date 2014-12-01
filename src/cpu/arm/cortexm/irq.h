#pragma once

#include "global.h"
#include "cpu/arm/cortexm/cortexutil.h"
#include "interface/irq/irq.h"

typedef enum irq_number
{
    __dummy_init_value__ = -15,
#define CORTEXM_DEFINE_FAULT(name) name ## _IRQn,
#include "cpu/arm/cortexm/fault_defs.h"
#undef CORTEXM_DEFINE_FAULT
#define CORTEXM_DEFINE_IRQ(name) name ## _IRQn,
#include CORTEXM_IRQ_DEF_FILE
#undef CORTEXM_DEFINE_IRQ
} IRQn_Type;

#ifdef __cplusplus
extern "C"
{
#endif
    extern void (*const _irqvectors[])() __attribute__((section(".vectors")));

    void unhandled_irq_handler();
#ifdef __cplusplus
}
#endif

