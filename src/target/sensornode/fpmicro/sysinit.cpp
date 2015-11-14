#include "global.h"
#include "app/sensornode/common.h"
#include "interface/clockgate/clockgate.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/f0/rcc_regs.h"
#include "soc/stm32/pwr_regs.h"
#include "sys/util.h"


void SENSORNODE_TARGET_OPTIMIZE sensornode_sysinit()
{
    GPIO::configure(SENSORNODE_LEDPIN, GPIO::MODE_OUTPUT, true);
    clockgate_enable(STM32_PWR_CLOCKGATE, true);
    union STM32_PWR_REG_TYPE::CR CR = { 0 };
    CR.b.DBP = true;
    CR.b.LPDS = true;
    STM32_PWR_REGS.CR.d32 = CR.d32;
    clockgate_enable(STM32_PWR_CLOCKGATE, false);
    union STM32_RCC_REG_TYPE::BDCR BDCR = { 0 };
    BDCR.b.LSEON = true;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
}

void SENSORNODE_TARGET_OPTIMIZE sensornode_reboot()
{
    reset();
}

void SENSORNODE_TARGET_OPTIMIZE sensornode_powerdown()
{
#ifndef DEBUG
    enter_critical_section();
    union STM32_RCC_REG_TYPE::BDCR BDCR = { 0 };
    BDCR.b.BDRST = true;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
    union STM32_RCC_REG_TYPE::CSR CSR = { 0 };
    STM32_RCC_REGS.CSR.d32 = CSR.d32;
    clockgate_enable(STM32_PWR_CLOCKGATE, true);
    union STM32_PWR_REG_TYPE::CR CR = { 0 };
    CR.b.LPDS = true;
    CR.b.CWUF = true;
    STM32_PWR_REGS.CR.d32 = CR.d32;
    clockgate_enable(STM32_PWR_CLOCKGATE, false);
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
#endif
    while (true) idle();
}
