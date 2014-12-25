#include "global.h"
#include "soc/stm32/f1/gpio.h"
#include "soc/stm32/f1/gpio_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


#ifndef STM32_GPIO_CONFIG
#define STM32_GPIO_CONFIG {}
#endif

using namespace STM32::GPIO_REGS;

void STM32::GPIO_F1::init()
{
    // Configure GPIO pins to their default values
    static const unsigned int gpiotable[][3] = STM32_GPIO_CONFIG;

    for (uint32_t i = 0; i < ARRAYLEN(gpiotable); i++)
    {
        bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(i), true);
        STM32_GPIO_REGS(i).CR.d32[0] = gpiotable[i][0];
        STM32_GPIO_REGS(i).CR.d32[1] = gpiotable[i][1];
        STM32_GPIO_REGS(i).BSRR.d32 = gpiotable[i][2];
        clockgate_enable(STM32_GPIO_CLOCKGATE(i), old);
    }
}

static void gpio_set_bits(int pin, uint32_t mask, uint32_t value)
{
    int port = pin >> 4;
    int shift = (pin & 0x7) << 2;
    volatile uint32_t* reg = &STM32_GPIO_REGS(port).CR.d32[(pin & 8) ? 1 : 0];
    bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(port), true);
    *reg = (*reg & ~(mask << shift)) | (value << shift);
    clockgate_enable(STM32_GPIO_CLOCKGATE(port), old);
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
}

void STM32::GPIO_F1::PinController::setMode(unsigned int pin, enum ::GPIO::mode mode) const
{
    uint32_t mask;
    uint32_t value;
    switch (mode)
    {
    case ::GPIO::MODE_ANALOG: value = 0x0; mask = 0xf; break;
    case ::GPIO::MODE_INPUT: value = 0x0; mask = 0x3; break;
    case ::GPIO::MODE_OUTPUT: value = 0x3; mask = 0xb; break;
    case ::GPIO::MODE_SPECIAL: value = 0x8; mask = 0x8; break;
    }
    gpio_set_bits(pin, mask, value);
}

void STM32::GPIO_F1::PinController::setType(unsigned int pin, enum ::GPIO::type type) const
{
    gpio_set_bits(pin, 4, type == ::GPIO::TYPE_OPENDRAIN ? 4 : 0);
}

void STM32::GPIO_F1::PinController::setPull(unsigned int pin, enum ::GPIO::pull pull) const
{
    if (pull != ::GPIO::PULL_NONE) setLevel(pin, pull == ::GPIO::PULL_UP);
    gpio_set_bits(pin, 0xc, pull == ::GPIO::PULL_NONE ? 4 : 8);
}

void STM32::GPIO_F1::PinController::setSpeed(unsigned int pin, enum STM32::GPIO_F1::Speed speed) const
{
    gpio_set_bits(pin, 0xc, speed);
}

void STM32::GPIO_F1::PinController::setSpecial(unsigned int pin, int function) const
{
}

const STM32::GPIO_F1::PinController STM32::GPIO_F1::Controller = STM32::GPIO_F1::PinController();
