#include "global.h"
#include "soc/stm32/f2/usb.h"
#include "core/dwotg/dwotg.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


constexpr STM32::USB::CoreRuntimeParams STM32::USB::coreRuntimeParams[];
STM32::USB* STM32::USB::activeInstance[STM32::USB_CORE_COUNT];


void STM32::USB::socEnableClocks()
{
    clockgate_enable(STM32::USB::coreRuntimeParams[core].clkgate, true);
}

void STM32::USB::socDisableClocks()
{
    clockgate_enable(STM32::USB::coreRuntimeParams[core].clkgate, false);
}

void STM32::USB::socEnableIrq()
{
    activeInstance[core] = this;
    irq_enable(STM32::USB::coreRuntimeParams[core].irq, true);
}

void STM32::USB::socDisableIrq()
{
    irq_enable(STM32::USB::coreRuntimeParams[core].irq, false);
}

void STM32::USB::socClearIrq()
{
    irq_clear_pending(STM32::USB::coreRuntimeParams[core].irq);
}

void STM32::USB::handleIrq(UsbCore core)
{
    if (activeInstance[core]) activeInstance[core]->handleIrq();
    else irq_enable(STM32::USB::coreRuntimeParams[core].irq, false);
}

extern "C" void otg_fs_irqhandler()
{
    STM32::USB::handleIrq(STM32::OTG_FS);
}

extern "C" void otg_hs_irqhandler()
{
    STM32::USB::handleIrq(STM32::OTG_HS);
}
