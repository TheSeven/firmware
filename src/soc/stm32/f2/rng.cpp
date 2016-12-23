#include "global.h"
#include "soc/stm32/f2/rng.h"
#include "soc/stm32/f2/rng_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


bool STM32::RNG::power(bool on)
{
    bool old = clockgate_enable(STM32_RNG_CLOCKGATE, true);
    union STM32_RNG_REG_TYPE::CR CR = { STM32_RNG_REGS.CR.d32 };
    bool result = CR.b.RNGEN;
    CR.b.RNGEN = on;
    STM32_RNG_REGS.CR.d32 = CR.d32;
    clockgate_enable(STM32_RNG_CLOCKGATE, old);
    return result;
}

uint32_t STM32::RNG::random()
{
    bool old = clockgate_enable(STM32_RNG_CLOCKGATE, true);
    while (!STM32_RNG_REGS.SR.b.DRDY);
    uint32_t result = STM32_RNG_REGS.DR;
    clockgate_enable(STM32_RNG_CLOCKGATE, old);
    return result;
}
