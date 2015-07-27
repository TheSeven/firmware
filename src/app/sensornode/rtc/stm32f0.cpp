#include "global.h"
#include "app/sensornode/rtc.h"
#include "sys/time.h"
#include "sys/util.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "cpu/arm/cortexm/irq.h"
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/f0/rcc_regs.h"
#include "soc/stm32/f0/rtc_regs.h"
#include "soc/stm32/exti_regs.h"
#include "soc/stm32/timer_regs.h"


class SensorNodeRTCSTM32 : public SensorNodeRTCDriver
{
    uint16_t rtcFrequency;
    int lastTime;
    int timeOffset;

    int getRTCFrequency();
    void reset();
    int getTime();
    void sleepUntil(int time);
};


SensorNodeRTCSTM32 stm32RTC;
SensorNodeRTCDriver* rtcDriver = &stm32RTC;


int SENSORNODE_RTC_OPTIMIZE SensorNodeRTCSTM32::getRTCFrequency()
{
    clockgate_enable(STM32_TIM14_CLOCKGATE, true);
    union STM32_TIM_REG_TYPE::OR OR = { 0 };
    OR.b.TI1_RMP = 1;
    STM32_TIM14_REGS.OR.d32 = OR.d32;
    union STM32_TIM_REG_TYPE::CCMR1 CCMR1 = { 0 };
    CCMR1.ic.CC1S = 1;
    CCMR1.ic.IC1PSC = 3;
    STM32_TIM14_REGS.CCMR1.d32 = CCMR1.d32;
    union STM32_TIM_REG_TYPE::CCER CCER = { 0 };
    CCER.b.CC1E = true;
    STM32_TIM14_REGS.CCER.d32 = CCER.d32;
    union STM32_TIM_REG_TYPE::CR1 CR1 = { 0 };
    STM32_TIM14_REGS.CR1.d32 = CR1.d32;
    STM32_TIM14_REGS.PSC = 0;
    STM32_TIM14_REGS.ARR = 0xffff;
    union STM32_TIM_REG_TYPE::EGR EGR = { 0 };
    EGR.b.UG = true;
    STM32_TIM14_REGS.EGR.d32 = EGR.d32;
    union STM32_TIM_REG_TYPE::SR SR = { 0 };
    SR.b.CC1IF = true;
    STM32_TIM14_REGS.SR.d32 = SR.d32;
    CR1.b.CEN = true;
    STM32_TIM14_REGS.CR1.d32 = CR1.d32;
    while (!STM32_TIM14_REGS.SR.b.CC1IF);
    int begin = STM32_TIM14_REGS.CCR1;
    STM32_TIM14_REGS.SR.d32 = SR.d32;
    while (!STM32_TIM14_REGS.SR.b.CC1IF);
    int end = STM32_TIM14_REGS.CCR1;
    STM32_TIM14_REGS.SR.d32 = SR.d32;
    STM32_TIM14_REGS.CR1.d32 = 0;
    clockgate_enable(STM32_TIM14_CLOCKGATE, false);
    return STM32_AHB_CLOCK * 8 / (end - begin);
}


void SENSORNODE_RTC_OPTIMIZE SensorNodeRTCSTM32::reset()
{
    while (!STM32_RCC_REGS.CSR.b.LSIRDY);
    union STM32_RCC_REG_TYPE::BDCR BDCR = { 0 };
    BDCR.b.BDRST = true;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
    BDCR.d32 = 0;
    BDCR.b.RTCEN = true;
    BDCR.b.RTCSEL = BDCR.b.RTCSEL_LSI;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
    STM32_RTC_REGS.WPR = 0xca;
    STM32_RTC_REGS.WPR = 0x53;
    union STM32_RTC_REG_TYPE::ISR ISR = { 0 };
    ISR.b.INIT = true;
    STM32_RTC_REGS.ISR.d32 = ISR.d32;
    while (!STM32_RTC_REGS.ISR.b.INITF);
    union STM32_RTC_REG_TYPE::PRER PRER = { 0 };
    rtcFrequency = (getRTCFrequency() >> 2) << 2;
    PRER.b.PREDIV_A = (1 << 2) - 1;
    PRER.b.PREDIV_S = (rtcFrequency >> 2) - 1;
    STM32_RTC_REGS.PRER.d32 = PRER.d32;
    STM32_RTC_REGS.DR.d32 = 0;
    STM32_RTC_REGS.TR.d32 = 0;
    STM32_RTC_REGS.ISR.d32 = 0;
    STM32_EXTI_REGS.RTSR = 1 << 17;
    STM32_EXTI_REGS.FTSR = 1 << 0;
    STM32_EXTI_REGS.IMR = (1 << 17) | (1 << 0);
    irq_enable(rtc_IRQn, true);
    irq_enable(exti0_1_IRQn, true);
}


int SENSORNODE_RTC_OPTIMIZE SensorNodeRTCSTM32::getTime()
{
    union STM32_RTC_REG_TYPE::TR TR = { STM32_RTC_REGS.TR.d32 };
    int time = ((TR.b.MNT * 10 + TR.b.MNU) * 6 + TR.b.ST) * 10 + TR.b.SU;
    if (time < lastTime) timeOffset += 3600;
    lastTime = time;
    return timeOffset + time;
}


void SENSORNODE_RTC_OPTIMIZE SensorNodeRTCSTM32::sleepUntil(int time)
{
    if (time <= getTime()) return;
    int seconds = (time - timeOffset) % 3600;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    union STM32_RTC_REG_TYPE::CR CR = { 0 };
    STM32_RTC_REGS.CR.d32 = CR.d32;
    while (!STM32_RTC_REGS.ISR.b.ALRAWF);
    union STM32_RTC_REG_TYPE::ALRMAR ALRMAR = { 0 };
    ALRMAR.b.MSK4 = true;
    ALRMAR.b.MSK3 = true;
    ALRMAR.b.MNT = minutes / 10;
    ALRMAR.b.MNU = minutes % 10;
    ALRMAR.b.ST = seconds / 10;
    ALRMAR.b.SU = seconds % 10;
    STM32_RTC_REGS.ALRMAR.d32 = ALRMAR.d32;
    CR.b.ALRAE = true;
    CR.b.ALRAIE = true;
    STM32_RTC_REGS.CR.d32 = CR.d32;
    enter_critical_section();
    while (time > getTime())
    {
#ifndef DEBUG
        SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
#endif
        idle();
        SCB->SCR = 0;
    }
    leave_critical_section();
}


extern "C" void SENSORNODE_RTC_OPTIMIZE rtc_irqhandler()
{
    STM32_RTC_REGS.CR.d32 = 0;
    STM32_RTC_REGS.ISR.d32 = 0;
    STM32_EXTI_REGS.PR = 1 << 17;
}


extern "C" void SENSORNODE_RTC_OPTIMIZE exti0_1_irqhandler()
{
    STM32_EXTI_REGS.PR = 1 << 0;
}
