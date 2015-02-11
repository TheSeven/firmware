#include "global.h"
#include "../output.h"
#include "soc/stm32/f0/rcc.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/timer_regs.h"
#include "soc/stm32/f1/dma_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


GPIO::Pin addrswitch[9] =
{
    PIN_A2,
    PIN_A6,
    PIN_A5,
    PIN_F0,
    PIN_F1,
    PIN_B1,
    PIN_A1,
    PIN_A0,
    PIN_A4,
};


void output_init()
{
    for (uint32_t i = 0; i < ARRAYLEN(addrswitch); i++)
    {
        GPIO::configure(addrswitch[i], GPIO::MODE_INPUT);
        GPIO::setPull(addrswitch[i], GPIO::PULL_UP);
    }
    clockgate_enable(STM32_TIM1_CLOCKGATE, true);
    clockgate_enable(STM32_TIM14_CLOCKGATE, true);
    union STM32_TIM_REG_TYPE::SMCR SMCR = { 0 };
    STM32_TIM1_REGS.SMCR.d32 = SMCR.d32;
    STM32_TIM14_REGS.SMCR.d32 = SMCR.d32;
    union STM32_TIM_REG_TYPE::CCER CCER = { 0 };
    STM32_TIM1_REGS.CCER.d32 = CCER.d32;
    STM32_TIM14_REGS.CCER.d32 = CCER.d32;
    union STM32_TIM_REG_TYPE::CCMR1 CCMR1 = { 0 };
    CCMR1.oc.OC2PE = true;
    CCMR1.oc.OC2M = 6;
    STM32_TIM1_REGS.CCMR1.d32 = CCMR1.d32;
    CCMR1.d32 = 0;
    CCMR1.oc.OC1PE = true;
    CCMR1.oc.OC1M = 6;
    STM32_TIM14_REGS.CCMR1.d32 = CCMR1.d32;
    union STM32_TIM_REG_TYPE::CCMR2 CCMR2 = { 0 };
    STM32_TIM14_REGS.CCMR2.d32 = CCMR2.d32;
    CCMR2.oc.OC3PE = true;
    CCMR2.oc.OC3M = 6;
    STM32_TIM1_REGS.CCMR2.d32 = CCMR2.d32;
    CCER.b.CC2E = true;
    CCER.b.CC3E = true;
    STM32_TIM1_REGS.CCER.d32 = CCER.d32;
    CCER.d32 = 0;
    CCER.b.CC1E = true;
    STM32_TIM14_REGS.CCER.d32 = CCER.d32;
    union STM32_TIM_REG_TYPE::BDTR BDTR = { 0 };
    BDTR.b.MOE = true;
    STM32_TIM1_REGS.BDTR.d32 = BDTR.d32;
    STM32_TIM14_REGS.BDTR.d32 = BDTR.d32;
    union STM32_TIM_REG_TYPE::CR2 TCR2 = { 0 };
    STM32_TIM1_REGS.CR2.d32 = TCR2.d32;
    STM32_TIM14_REGS.CR2.d32 = TCR2.d32;
    STM32_TIM1_REGS.PSC = 0;
    STM32_TIM14_REGS.PSC = 0;
    STM32_TIM1_REGS.ARR = 255;
    STM32_TIM14_REGS.ARR = 255;
    union STM32_TIM_REG_TYPE::EGR EGR = { 0 };
    STM32_TIM1_REGS.CCR3 = 0;
    STM32_TIM1_REGS.CCR2 = 0;
    STM32_TIM14_REGS.CCR1 = 0;
    EGR.b.UG = true;
    STM32_TIM1_REGS.EGR.d32 = EGR.d32;
    STM32_TIM14_REGS.EGR.d32 = EGR.d32;
    union STM32_TIM_REG_TYPE::CR1 TCR1 = { 0 };
    TCR1.b.CEN = true;
    STM32_TIM1_REGS.CR1.d32 = TCR1.d32;
    STM32_TIM14_REGS.CR1.d32 = TCR1.d32;
}

void output_handle_dmx(uint8_t* data)
{
    uint32_t addr = 0;
    for (uint32_t i = 0; i < ARRAYLEN(addrswitch); i++) addr = (addr << 1) | !GPIO::getLevel(addrswitch[i]);
    if (!addr) addr = 512;
    STM32_TIM1_REGS.CCR2 = addr > 511 ? 0 : data[addr - 1];
    STM32_TIM1_REGS.CCR3 = addr > 510 ? 0 : data[addr];
    STM32_TIM14_REGS.CCR1 = addr > 509 ? 0 : data[addr + 1];
}
