#include "global.h"
#include "soc/stm32/f2/usb.h"
#include "core/dwotg/dwotg.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"
#include "usb.h"


constexpr STM32::USB::CoreRuntimeParams STM32::USB::coreRuntimeParams[];
static STM32::USB* stm32_usb_instance[STM32::USB_CORE_COUNT];


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
    stm32_usb_instance[core] = this;
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

extern "C" void otg_fs_irqhandler()
{
    if (stm32_usb_instance[STM32::OTG_FS]) stm32_usb_instance[STM32::OTG_FS]->handleIrq();
    else irq_enable(otg_fs_IRQn, false);
}

extern "C" void otg_hs_irqhandler()
{
    if (stm32_usb_instance[STM32::OTG_HS]) stm32_usb_instance[STM32::OTG_HS]->handleIrq();
    else irq_enable(otg_hs_IRQn, false);
}
