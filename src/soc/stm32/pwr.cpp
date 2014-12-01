#include "global.h"
#include "soc/stm32/pwr.h"
#include "soc/stm32/pwr_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


namespace STM32
{

    void PWR::init()
    {
        // Set up some sane defaults
        bool old = clockgate_enable(STM32_PWR_CLOCKGATE, true);
        union STM32_PWR_REG_TYPE::CR CR = { 0 };
        CR.b.LPDS = true;
#if !defined(SOC_STM32F0) && !defined(SOC_STM32F1)
        CR.b.FPDS = true;
#ifdef SOC_STM32F4
        CR.b.PMODE = 4 - STM32_VOLTAGE_SCALE;
#endif
#ifdef SOC_STM32F42X
        CR.b.UDEN = 3;
        CR.b.MRUDS = true;
        CR.b.LPUDS = true;
#ifdef SOC_STM32F42X
#ifdef STM32_OVERDRIVE
        CR.b.ODEN = true;
#endif
#endif
#endif
#endif
        STM32_PWR_REGS.CR.d32 = CR.d32;
#if !defined(SOC_STM32F0) && !defined(SOC_STM32F1)
        union STM32_PWR_REG_TYPE::CSR CSR = { 0 };
        CSR.b.BRE = true;
        STM32_PWR_REGS.CSR.d32 = CSR.d32;
#endif

#if defined(SOC_STM32F42X) && defined(STM32_OVERDRIVE)
        while (!STM32_PWR_REGS.CSR.b.ODRDY);
        STM32_PWR_REGS.CR.b.ODSW = true;
#endif
        clockgate_enable(STM32_PWR_CLOCKGATE, old);
    }

}

