#include "global.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/gpio_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


#ifndef STM32_GPIO_CONFIG
#define STM32_GPIO_CONFIG {}
#endif

namespace STM32
{
    using namespace STM32::GPIO_REGS;

    void GPIO::init()
    {
        // Configure GPIO pins to their default values
        static const unsigned int gpiotable[][7] = STM32_GPIO_CONFIG;

        for (uint32_t i = 0; i < ARRAYLEN(gpiotable); i++)
        {
            bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(i), true);
            STM32_GPIO_REGS(i).AFR.d32[0] = gpiotable[i][6];
            STM32_GPIO_REGS(i).AFR.d32[1] = gpiotable[i][5];
            STM32_GPIO_REGS(i).BSRR.d32 = gpiotable[i][4];
            STM32_GPIO_REGS(i).PUPDR.d32 = gpiotable[i][3];
            STM32_GPIO_REGS(i).OSPEEDR.d32 = gpiotable[i][2];
            STM32_GPIO_REGS(i).OTYPER.d32 = gpiotable[i][1];
            STM32_GPIO_REGS(i).MODER.d32 = gpiotable[i][0];
            clockgate_enable(STM32_GPIO_CLOCKGATE(i), old);
        }
    }

    static void __attribute__((optimize("-Os"))) gpio_set_bits(volatile uint32_t* reg, unsigned int pin,
                                                               uint32_t value, unsigned int width)
    {
        int port = pin >> 4;
        pin = pin & 0xf;
        reg += port << 8;
        uint32_t mask = 0xffffffff >> (32 - width);
        uint32_t shift = width * pin;
        bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(port), true);
        *reg = (*reg & ~(mask << shift)) | (value << shift);
        clockgate_enable(STM32_GPIO_CLOCKGATE(port), old);
    }

    bool __attribute__((optimize("-Os"))) GPIO::PinController::getLevel(unsigned int pin) const
    {
        bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), true);
        bool result = (STM32_GPIO_REGS(pin >> 4).IDR.d32 >> (pin & 0xf)) & 1;
        clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), old);
        return result;
    }
    
    void __attribute__((optimize("-Os"))) GPIO::PinController::setLevel(unsigned int pin, bool level) const
    {
        bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), true);
        STM32_GPIO_REGS(pin >> 4).BSRR.d32 = (0x10000 | !!level) << (pin & 0xf);
        clockgate_enable(STM32_GPIO_CLOCKGATE(pin >> 4), old);
    }
    
    void __attribute__((optimize("-Os"))) GPIO::PinController::setMode(unsigned int pin, enum ::GPIO::mode mode) const
    {
        gpio_set_bits(&STM32_GPIOA_REGS.MODER.d32, pin, (uint32_t)mode, 2);
    }
    
    void __attribute__((optimize("-Os"))) GPIO::PinController::setType(unsigned int pin, enum ::GPIO::type type) const
    {
        gpio_set_bits(&STM32_GPIOA_REGS.OTYPER.d32, pin, (uint32_t)type, 1);
    }
    
    void __attribute__((optimize("-Os"))) GPIO::PinController::setPull(unsigned int pin, enum ::GPIO::pull pull) const
    {
        gpio_set_bits(&STM32_GPIOA_REGS.PUPDR.d32, pin, (uint32_t)pull, 2);
    }
    
    void __attribute__((optimize("-Os"))) GPIO::PinController::setSpeed(unsigned int pin, enum Speed speed) const
    {
        gpio_set_bits(&STM32_GPIOA_REGS.OSPEEDR.d32, pin, (uint32_t)speed, 2);
    }
    
    void __attribute__((optimize("-Os"))) GPIO::PinController::setSpecial(unsigned int pin, enum Special function) const
    {
        gpio_set_bits(&STM32_GPIOA_REGS.AFR.d32[(pin >> 3) & 1], pin & ~8, (uint32_t)function, 4);
    }
    
    void __attribute__((optimize("-Os"))) GPIO::PinController::setSpecial(unsigned int pin, int function) const
    {
        setSpecial(pin, (enum Special)(function & 0xf));
    }
    
    const GPIO::PinController GPIO::Controller;

}
