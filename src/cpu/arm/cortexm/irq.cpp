#include "global.h"
#include "cpu/arm/cortexm/irq.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "interface/irq/irq.h"
#include "sys/util.h"

void irq_enable(int irq, bool on)
{
    if (on) NVIC_EnableIRQ((IRQn_Type)irq);
    else NVIC_DisableIRQ((IRQn_Type)irq);
}

bool irq_get_pending(int irq)
{
    return NVIC_GetPendingIRQ((IRQn_Type)irq);
}

void irq_clear_pending(int irq)
{
    NVIC_ClearPendingIRQ((IRQn_Type)irq);
}

void irq_set_pending(int irq)
{
#ifdef CPU_ARM_CORTEX_M0
    NVIC_SetPendingIRQ((IRQn_Type)irq);
#else
    NVIC->STIR = irq;
#endif
}

void irq_set_priority(int irq, int priority)
{
    NVIC_SetPriority((IRQn_Type)irq, priority);
}

#ifndef CPU_ARM_CORTEX_M0
void irq_set_priority_grouping(int bits)
{
    NVIC_SetPriorityGrouping(bits);
}
#endif
