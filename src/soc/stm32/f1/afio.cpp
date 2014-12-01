#include "global.h"
#include "soc/stm32/f1/afio.h"
#include "soc/stm32/f1/afio_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


namespace STM32
{

    void AFIO::init()
    {
        bool old = clockgate_enable(STM32_AFIO_CLOCKGATE, true);
        union STM32_AFIO_REG_TYPE::MAPR MAPR = { 0 };
        MAPR.b.SWJ_CFG = MAPR.b.SWJ_SWI_ONLY;
        STM32_AFIO_REGS.MAPR.d32 = MAPR.d32;
        union STM32_AFIO_REG_TYPE::MAPR2 MAPR2 = { 0 };
        STM32_AFIO_REGS.MAPR2.d32 = MAPR2.d32;
        clockgate_enable(STM32_AFIO_CLOCKGATE, old);
    }

    void AFIO::setMIIMode(bool rmii)
    {
        bool old = clockgate_enable(STM32_AFIO_CLOCKGATE, true);
        STM32_AFIO_REGS.MAPR.b.MII_RMII = (typeof(STM32_AFIO_REGS.MAPR.b.MII_RMII))rmii;
        clockgate_enable(STM32_AFIO_CLOCKGATE, old);
    }
}
