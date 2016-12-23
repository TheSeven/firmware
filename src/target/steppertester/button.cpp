#include "global.h"
#include "button.h"
#include "cpu/arm/cortexm/irq.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/pwr_regs.h"
#include "soc/stm32/gpio_regs.h"
#include "soc/stm32/exti_regs.h"
#include "interface/irq/irq.h"
#include "sys/util.h"
#include "sys/time.h"


#define DEBOUNCE_TIME 50000
#define REPEAT_DELAY 400000


static uint32_t debounceTime[3];
static uint16_t buttonDown;
static uint8_t buttonEvent;
static bool repeating;


static uint32_t getRawButtonState()
{
    return ~STM32_GPIOA_REGS.IDR.d32;
}


uint32_t getButtonState()
{
    uint32_t raw = getRawButtonState();
    uint32_t pressed = 0;
    if (raw & 0x0200) pressed |= 1;
    if (raw & 0x0008) pressed |= 2;
    if (raw & 0x0400) pressed |= 4;
    return pressed;
}

extern "C" __attribute__((alias("exti2_3_irqhandler"))) void exti4_15_irqhandler();
extern "C" void exti2_3_irqhandler()
{
    uint32_t PR = STM32_EXTI_REGS.PR;
    STM32_EXTI_REGS.PR = PR;
    uint32_t pressed = getRawButtonState();
    uint32_t change = pressed ^ buttonDown;
    uint32_t down = pressed & change;
    uint32_t now = read_usec_timer();
    uint32_t events = 0;
    if ((down & 0x0200) && now - debounceTime[0] > DEBOUNCE_TIME) events |= 1;
    if ((down & 0x0008) && now - debounceTime[1] > DEBOUNCE_TIME) events |= 2;
    if ((down & 0x0400) && now - debounceTime[2] > DEBOUNCE_TIME) events |= 4;
    if (change & 0x0200) debounceTime[0] = now;
    if (change & 0x0008) debounceTime[1] = now;
    if (change & 0x0400) debounceTime[2] = now;
    buttonDown = pressed;
    buttonEvent = events;
}


void initButton()
{
    STM32_EXTI_REGS.FTSR = 0x0608;
    STM32_EXTI_REGS.RTSR = 0x0608;
    STM32_EXTI_REGS.IMR = 0x0608;
    irq_set_priority(exti2_3_IRQn, 1);
    irq_set_priority(exti4_15_IRQn, 1);
    irq_enable(exti2_3_IRQn, true);
    irq_enable(exti4_15_IRQn, true);
}


uint32_t waitButton(uint32_t timeout)
{
    timeout = TIMEOUT_SETUP(timeout);
    while (!buttonEvent && !TIMEOUT_EXPIRED(timeout));
    enter_critical_section();
    uint32_t events = buttonEvent;
    buttonEvent = 0;
    leave_critical_section();
    return events;
}


uint32_t waitButtonRepeat(uint32_t interval)
{
    uint32_t pressed = waitButton(repeating ? interval : REPEAT_DELAY);
    if (!pressed)
    {
        pressed = getButtonState();
        repeating = pressed;
    }
    return pressed;
}
