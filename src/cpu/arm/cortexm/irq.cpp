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

void irq_clear_pending(int irq)
{
    NVIC_ClearPendingIRQ((IRQn_Type)irq);
}
