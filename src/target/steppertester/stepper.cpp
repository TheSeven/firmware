#include "global.h"
#include "stepper.h"
#include "soc/stm32/gpio_regs.h"
#include "soc/stm32/timer_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "storage.h"


#define STEPPER_MIN_SPEED 50
#define STEPPER_HARDSTOP_MAX_SPEED 100


volatile bool wiggle;
volatile bool running;
volatile int8_t dir;
volatile int speed;
volatile int pos;
volatile int targetpos;


void setMicrostepping(uint32_t mode)
{
    mode = MIN(mode, cfg.data.driver == 0 ? 4 : 5);
    cfg.data.microstepping = mode;
    if (cfg.data.driver == 0 && mode == 4) mode = 7;
    STM32_GPIOA_REGS.BSRR.d32 = 0x70000 | mode;
}


static void setDirection(int direction)
{
    dir = direction;
    STM32_GPIOA_REGS.BSRR.d32 = 0x100000 | ((dir > 0) << 4);
}


static void start()
{
    union STM32_TIM_REG_TYPE::EGR egr = { 0 };
    egr.b.UG = true;
    STM32_TIM16_REGS.EGR.d32 = egr.d32;
    union STM32_TIM_REG_TYPE::CR1 cr1 = { 0 };
    cr1.b.CEN = true;
    cr1.b.ARPE = true;
    STM32_TIM16_REGS.CR1.d32 = cr1.d32;
    running = true;
}


static void stop()
{
    union STM32_TIM_REG_TYPE::CR1 cr1 = { 0 };
    STM32_TIM16_REGS.CR1.d32 = cr1.d32;
    running = false;
    speed = 0;
    if (wiggle)
    {
        if (!targetpos) targetpos = cfg.data.wiggleTo;
        else targetpos = 0;
    }
}

static void setSpeed(int freq)
{
    if (freq < STEPPER_MIN_SPEED)
    {
        stop();
        return;
    }
    int counter = (1536000000 >> cfg.data.microstepping) / freq;
    int pulse = 100;
    int prescaler = 1;
    while (counter > 65536)
    {
        counter >>= 1;
        pulse >>= 1;
        prescaler <<= 1;
    }
    bool needStart = !running;
    if (running)
    {
        int timeLeft = ((int)STM32_TIM16_REGS.ARR - (int)STM32_TIM16_REGS.CNT) * ((int)STM32_TIM16_REGS.PSC + 1);
        if (timeLeft < 500) return;
        if (timeLeft > 48000)
        {
            if (STM32_TIM16_REGS.CNT < STM32_TIM16_REGS.CCR1 * 2) return;
            if (STM32_TIM16_REGS.CNT * (STM32_TIM16_REGS.PSC + 1) < (uint32_t)(1000000 >> cfg.data.microstepping))
                return;
            needStart = true;
        }
    }
    STM32_TIM16_REGS.PSC = prescaler - 1;
    STM32_TIM16_REGS.ARR = counter - 1;
    STM32_TIM16_REGS.CCR1 = pulse + 1;
    if (needStart) start();
}


extern "C" void tim14_irqhandler()
{
    STM32_TIM14_REGS.SR.d32 = 0;
    int mask = 0x1f >> cfg.data.microstepping;
    if ((pos & ~mask) == (targetpos & ~mask) && speed <= STEPPER_HARDSTOP_MAX_SPEED * 2)
    {
        stop();
        return;
    }
    int distance = targetpos - pos;
    int left = distance * dir;
    int accel = dir > 0 ? cfg.data.accelFwd : cfg.data.accelBwd;
    int brake = dir > 0 ? cfg.data.brakeFwd : cfg.data.brakeBwd;
    int brakedist = speed / 100 * speed / brake / 20;
    if (brakedist >= left) speed -= brake;
    else speed += accel;
    int maxspeed = cfg.data.maxSpeed;
    if (speed > maxspeed) speed = maxspeed;
    if (speed < STEPPER_MIN_SPEED)
    {
        if (left < 0) setDirection(dir * -1);
        speed = STEPPER_MIN_SPEED;
    }
    setSpeed(speed);
}


extern "C" void tim16_irqhandler()
{
    STM32_TIM16_REGS.SR.d32 = 0;
    pos += dir << (5 - cfg.data.microstepping);
}


void initStepper()
{
    setMicrostepping(cfg.data.microstepping);
    setDirection(1);
    clockgate_enable(STM32_TIM14_CLOCKGATE, true);
    clockgate_enable(STM32_TIM16_CLOCKGATE, true);
    union STM32_TIM_REG_TYPE::CR1 cr1 = { 0 };
    cr1.b.ARPE = true;
    STM32_TIM16_REGS.CR1.d32 = cr1.d32;
    union STM32_TIM_REG_TYPE::CR2 cr2 = { 0 };
    STM32_TIM16_REGS.CR2.d32 = cr2.d32;
    union STM32_TIM_REG_TYPE::DIER dier = { 0 };
    dier.b.UIE = true;
    STM32_TIM16_REGS.DIER.d32 = dier.d32;
    union STM32_TIM_REG_TYPE::CCMR1 ccmr1 = { 0 };
    ccmr1.oc.OC1PE = true;
    ccmr1.oc.OC1M = 6;
    STM32_TIM16_REGS.CCMR1.d32 = ccmr1.d32;
    union STM32_TIM_REG_TYPE::CCER ccer = { 0 };
    ccer.b.CC1E = true;
    STM32_TIM16_REGS.CCER.d32 = ccer.d32;
    union STM32_TIM_REG_TYPE::BDTR bdtr = { 0 };
    bdtr.b.MOE = true;
    STM32_TIM16_REGS.BDTR.d32 = bdtr.d32;
    cr1 = { 0 };
    STM32_TIM14_REGS.CR1.d32 = cr1.d32;
    cr2 = { 0 };
    STM32_TIM14_REGS.CR2.d32 = cr2.d32;
    dier = { 0 };
    dier.b.UIE = true;
    STM32_TIM14_REGS.DIER.d32 = dier.d32;
    ccmr1 = { 0 };
    ccmr1.oc.OC1M = 5;
    STM32_TIM14_REGS.CCMR1.d32 = ccmr1.d32;
    ccer = { 0 };
    ccer.b.CC1E = true;
    ccer.b.CC1P = true;
    STM32_TIM14_REGS.CCER.d32 = ccer.d32;
    STM32_TIM14_REGS.PSC = 0;
    STM32_TIM14_REGS.ARR = 47999;
    irq_set_priority(tim14_IRQn, 2);
    irq_set_priority(tim16_IRQn, 3);
    irq_enable(tim14_IRQn, true);
    irq_enable(tim16_IRQn, true);
    union STM32_TIM_REG_TYPE::EGR egr = { 0 };
    egr.b.UG = true;
    STM32_TIM14_REGS.EGR.d32 = egr.d32;
    cr1 = { 0 };
    cr1.b.CEN = true;
    STM32_TIM14_REGS.CR1.d32 = cr1.d32;
}
