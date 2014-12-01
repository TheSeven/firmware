#include "global.h"
#include "interface/gpio/gpio.h"
#include "sys/util.h"
#include GPIO_STATIC_CONTROLLER_HEADER

#ifdef GPIO_DYNAMIC_CONTROLLERS
GPIO::PinController* gpio_controllers[GPIO_DYNAMIC_CONTROLLERS + 1] =
{
    &GPIO_STATIC_CONTROLLER
};
#define GPIO_CONTROLLER (*gpio_controllers[pin.controller])
#else
#define GPIO_CONTROLLER GPIO_STATIC_CONTROLLER
#endif

bool __attribute__((optimize("-Os"))) GPIO::getLevel(Pin pin)
{
    return GPIO_CONTROLLER.getLevel(pin.pin);
}

void __attribute__((optimize("-Os"))) GPIO::setLevel(Pin pin, bool level)
{
    GPIO_CONTROLLER.setLevel(pin.pin, level);
}

void __attribute__((optimize("-Os"))) GPIO::setMode(Pin pin, enum mode mode)
{
    GPIO_CONTROLLER.setMode(pin.pin, mode);
}

void __attribute__((optimize("-Os"))) GPIO::setType(Pin pin, enum type type)
{
    GPIO_CONTROLLER.setType(pin.pin, type);
}

void __attribute__((optimize("-Os"))) GPIO::setPull(Pin pin, enum pull pull)
{
    GPIO_CONTROLLER.setPull(pin.pin, pull);
}

void __attribute__((optimize("-Os"))) GPIO::setSpecial(Pin pin, int function)
{
    GPIO_CONTROLLER.setSpecial(pin.pin, function);
}

void __attribute__((optimize("-Os"))) GPIO::configure(Pin pin, enum mode mode, bool level)
{
    GPIO_CONTROLLER.setLevel(pin.pin, level);
    GPIO_CONTROLLER.setMode(pin.pin, mode);
}

void __attribute__((optimize("-Os"))) GPIO::configure(Pin pin, int function)
{
    GPIO_CONTROLLER.setSpecial(pin.pin, function);
    GPIO_CONTROLLER.setMode(pin.pin, GPIO::MODE_SPECIAL);
}

void __attribute__((optimize("-Os"))) GPIO::configure(Pin pin, enum mode mode, bool level, enum type type, enum pull pull, int function)
{
    GPIO_CONTROLLER.setLevel(pin.pin, level);
    GPIO_CONTROLLER.setSpecial(pin.pin, function);
    GPIO_CONTROLLER.setType(pin.pin, type);
    GPIO_CONTROLLER.setPull(pin.pin, pull);
    GPIO_CONTROLLER.setMode(pin.pin, mode);
}

