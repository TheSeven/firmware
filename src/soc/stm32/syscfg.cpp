#include "global.h"
#include "soc/stm32/syscfg.h"
#include "soc/stm32/syscfg_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


namespace STM32
{

    void SYSCFG::init()
    {
        // Configure number of waitstates correctly
        bool old = clockgate_enable(STM32_SYSCFG_CLOCKGATE, true);
        union STM32_SYSCFG_REG_TYPE::MEMRMP MEMRMP = { 0 };
        MEMRMP.b.MEM_MODE = 0;
        STM32_SYSCFG_REGS.MEMRMP.d32 = MEMRMP.d32;
        clockgate_enable(STM32_SYSCFG_CLOCKGATE, old);
    }

#ifdef STM32_ENABLE_ETHERNET
    void SYSCFG::setMIIMode(bool rmii)
    {
        bool old = clockgate_enable(STM32_SYSCFG_CLOCKGATE, true);
        STM32_SYSCFG_REGS.PMC.b.MII_RMII = (typeof(STM32_SYSCFG_REGS.PMC.b.MII_RMII))rmii;
        clockgate_enable(STM32_SYSCFG_CLOCKGATE, old);
    }
#endif
}
