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
#include "soc/stm32/syscfg_regs.h"


class SensorNodeRTCSTM32 : public SensorNodeRTCDriver
{
public:
    uint16_t calibInterval = 300;

private:
    uint16_t rtcFrequency;
    int lastTime;
    int timeOffset;

    void reset();
    int getTime();
    void sleepUntil(int time);
    void getState(RTCState* state);
};


SensorNodeRTCSTM32 stm32RTC;
SensorNodeRTCDriver* rtcDriver = &stm32RTC;


void SENSORNODE_RTC_OPTIMIZE SensorNodeRTCSTM32::reset()
{
    bool old = clockgate_enable(STM32_SYSCFG_CLOCKGATE, true);
    STM32_SYSCFG_REGS.EXTICR[3] = 0x20;
    clockgate_enable(STM32_SYSCFG_CLOCKGATE, old);
    while (!STM32_RCC_REGS.BDCR.b.LSERDY);
    union STM32_RCC_REG_TYPE::BDCR BDCR = { STM32_RCC_REGS.BDCR.d32 };
    BDCR.b.BDRST = true;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
    BDCR.b.BDRST = false;
    BDCR.b.RTCEN = true;
    BDCR.b.RTCSEL = BDCR.b.RTCSEL_LSE;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
    STM32_RTC_REGS.WPR = 0xca;
    STM32_RTC_REGS.WPR = 0x53;
    union STM32_RTC_REG_TYPE::ISR ISR = { 0 };
    ISR.b.INIT = true;
    STM32_RTC_REGS.ISR.d32 = ISR.d32;
    while (!STM32_RTC_REGS.ISR.b.INITF);
    union STM32_RTC_REG_TYPE::PRER PRER = { 0 };
    PRER.b.PREDIV_A = (1 << 2) - 1;
    PRER.b.PREDIV_S = (32768 >> 2) - 1;
    STM32_RTC_REGS.PRER.d32 = PRER.d32;
    STM32_RTC_REGS.DR.d32 = 0;
    STM32_RTC_REGS.TR.d32 = 0;
    STM32_RTC_REGS.ISR.d32 = 0;
    STM32_EXTI_REGS.RTSR = 1 << 17;
    STM32_EXTI_REGS.FTSR = 1 << 13;
    STM32_EXTI_REGS.IMR = (1 << 17) | (1 << 13);
    irq_enable(rtc_IRQn, true);
    irq_enable(exti4_15_IRQn, true);
    lastTime = 0;
    timeOffset = 0;
}


int SENSORNODE_RTC_OPTIMIZE SensorNodeRTCSTM32::getTime()
{
    union STM32_RTC_REG_TYPE::TR TR = { STM32_RTC_REGS.TR.d32 };
    int time = ((TR.b.MNT * 10 + TR.b.MNU) * 6 + TR.b.ST) * 10 + TR.b.SU;
    if (time < lastTime) timeOffset += 3600;
    lastTime = time;
    time += timeOffset;
    return time;
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
    if (time > getTime())
    {
#ifndef DEBUG
        SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
#endif
        idle();
        SCB->SCR = 0;
    }
}


void SensorNodeRTCSTM32::getState(RTCState* state)
{
    state->frequency = 32768;
    state->subsecond = 32768 - (STM32_RTC_REGS.SSR << 2);
    state->time = getTime();
    discard(STM32_RTC_REGS.DR.d32);
}


extern "C" void SENSORNODE_RTC_OPTIMIZE rtc_irqhandler()
{
    STM32_RTC_REGS.CR.d32 = 0;
    STM32_RTC_REGS.ISR.d32 = 0;
    STM32_EXTI_REGS.PR = 1 << 17;
}


extern "C" void SENSORNODE_RTC_OPTIMIZE exti4_15_irqhandler()
{
    STM32_EXTI_REGS.PR = 1 << 13;
}
