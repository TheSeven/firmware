#include "global.h"
#include "soc/stm32/syscfg.h"
#include "soc/stm32/syscfg_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


namespace STM32
{

    void SYSCFG::init()
    {
        clockgate_enable(STM32_SYSCFG_CLOCKGATE, true);
        union STM32_SYSCFG_REG_TYPE::CFGR1 CFGR1 = { 0 };
        CFGR1.b.MEM_MODE = 0;
        STM32_SYSCFG_REGS.CFGR1.d32 = CFGR1.d32;
        clockgate_enable(STM32_SYSCFG_CLOCKGATE, false);
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
