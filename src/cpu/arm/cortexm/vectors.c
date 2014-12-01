#include "global.h"
#include "cpu/arm/cortexm/irq.h"
#include "cpu/arm/cortexm/init.h"
#include "cpu/arm/cortexm/cortexutil.h"
#include "sys/util.h"


__attribute__((weak)) int fault_handler()
{
    hang();
}

__attribute__((weak)) void unhandled_irq_handler()
{
    fault_handler();
}

#define CORTEXM_DEFINE_FAULT(name) __attribute__((weak,alias("fault_handler"))) void name ## _faulthandler();
#include "cpu/arm/cortexm/fault_defs.h"
#undef CORTEXM_DEFINE_FAULT
#define CORTEXM_DEFINE_IRQ(name) __attribute__((weak,alias("unhandled_irq_handler"))) void name ## _irqhandler();
#include CORTEXM_IRQ_DEF_FILE
#undef CORTEXM_DEFINE_IRQ

void _stack_end();  // Ugly hack: Not actually a function, but a symbol defined in the linker script

void (*const _irqvectors[])() __attribute__((used,section(".vectors"))) =
{
    _stack_end,
    init,
#define CORTEXM_DEFINE_FAULT(name) name ## _faulthandler,
#include "cpu/arm/cortexm/fault_defs.h"
#undef CORTEXM_DEFINE_FAULT
#define CORTEXM_DEFINE_IRQ(name) name ## _irqhandler,
#include CORTEXM_IRQ_DEF_FILE
#undef CORTEXM_DEFINE_IRQ
};

