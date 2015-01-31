#include "global.h"
#include "soc/stm32/f1/gpio.h"
#include "soc/stm32/f1/gpio_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


#ifndef STM32_GPIO_CONFIG
#define STM32_GPIO_CONFIG {}
#endif

using namespace STM32::GPIO_REGS;


struct stm32_f1_gpio_state
{
    bool level : 1;
    enum GPIO::pull pull : 2;
    enum GPIO::type type : 1;
    enum GPIO::mode mode : 2;
    enum STM32::GPIO_F1::Speed speed : 2;
};


static struct stm32_f1_gpio_state state[] = STM32_GPIO_CONFIG;


void update_pin(int pin)
{
    uint32_t mode;
    uint32_t level = state[pin].level;
    switch (state[pin].mode)
    {
    case ::GPIO::MODE_ANALOG:
        mode = 0x0;
        break;
    case ::GPIO::MODE_INPUT:
        if (state[pin].pull != GPIO::PULL_NONE)
        {
            mode = 0x8;
            level = state[pin].pull == GPIO::PULL_UP;
        }
        else mode = 0x4;
        break;
    default:
        mode = state[pin].speed;
        if (state[pin].type == ::GPIO::TYPE_OPENDRAIN) mode |= 0x4;
        if (state[pin].mode == ::GPIO::MODE_SPECIAL) mode |= 0x8;
    }
    int port = pin >> 4;
    int shift = (pin & 0x7) << 2;
    volatile uint32_t* reg = &STM32_GPIO_REGS(port).CR.d32[(pin & 8) ? 1 : 0];
    bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(port), true);
    if (state[pin].mode != ::GPIO::MODE_INPUT) STM32_GPIO_REGS(pin >> 4).BSRR.d32 = (0x10000 | level) << (pin & 0xf);
    *reg = (*reg & ~(0xf << shift)) | (mode << shift);
    if (state[pin].mode == ::GPIO::MODE_INPUT) STM32_GPIO_REGS(pin >> 4).BSRR.d32 = (0x10000 | level) << (pin & 0xf);
    clockgate_enable(STM32_GPIO_CLOCKGATE(port), old);
}

void STM32::GPIO_F1::init()
{
    // Configure GPIO pins to their default values
    for (uint32_t i = 0; i < ARRAYLEN(state); i++) update_pin(i);
}

bool STM32::GPIO_F1::PinController::getLevel(unsigned int pin) const
{
    bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), true);
    bool result = ((STM32_GPIO_REGS(pin >> 4).IDR.d32 >> (pin & 0xf))) & 1;
    clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), old);
    return result;
}

void STM32::GPIO_F1::PinController::setLevel(unsigned int pin, bool level) const
{
    bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), true);
    STM32_GPIO_REGS(pin >> 4).BSRR.d32 = (0x10000 | !!level) << (pin & 0xf);
    clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), old);
    state[pin].level = level;
}

void STM32::GPIO_F1::PinController::setMode(unsigned int pin, enum ::GPIO::mode mode) const
{
    state[pin].mode = mode;
    update_pin(pin);
}

void STM32::GPIO_F1::PinController::setType(unsigned int pin, enum ::GPIO::type type) const
{
    state[pin].type = type;
    update_pin(pin);
}

void STM32::GPIO_F1::PinController::setPull(unsigned int pin, enum ::GPIO::pull pull) const
{
    state[pin].pull = pull;
    update_pin(pin);
}

void STM32::GPIO_F1::PinController::setSpeed(unsigned int pin, enum STM32::GPIO_F1::Speed speed) const
{
    state[pin].speed = speed;
    update_pin(pin);
}

void STM32::GPIO_F1::PinController::setSpecial(unsigned int pin, int function) const
{
}

const STM32::GPIO_F1::PinController STM32::GPIO_F1::Controller;
