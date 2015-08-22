#include "global.h"
#include "app/sensornode/common.h"
#include "sys/time.h"
#include "sys/util.h"
#include "interface/clockgate/clockgate.h"
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/gpio_regs.h"
#include "device/ds1820/ds1820.h"


bool ONEWIRE_OPTIMIZE OneWire::Bus::reset() const
{
    GPIO::setPull(pin, GPIO::PULL_UP);
    int time = TIMEOUT_SETUP(1000);
    while (!GPIO::getLevel(pin)) if (TIMEOUT_EXPIRED(time)) return false;
    time = delay(read_usec_timer() + 100);
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 0x10000 << (pin.pin & 0xf);
    time = delay(time + 480);
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 1 << (pin.pin & 0xf);
    time = delay(time + 70);
    bool result = !((STM32_GPIO_REGS(pin.pin >> 4).IDR.d32 >> (pin.pin & 0xf)) & 1);
    delay(time + 410);
    return result;
}


void ONEWIRE_OPTIMIZE OneWire::Bus::writeBit(bool bit) const
{
    int time = read_usec_timer();
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 0x10000 << (pin.pin & 0xf);
    time = delay(time + (bit ? 5 : 60));
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 1 << (pin.pin & 0xf);
    delay(time + (bit ? 55 : 5));
}


bool ONEWIRE_OPTIMIZE OneWire::Bus::readBit() const
{
    int time = read_usec_timer();
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 0x10000 << (pin.pin & 0xf);
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 1 << (pin.pin & 0xf);
    time = delay(time + 14);
    bool result = (STM32_GPIO_REGS(pin.pin >> 4).IDR.d32 >> (pin.pin & 0xf)) & 1;
    delay(time + 50);
    return result;
}


void DS1820_OPTIMIZE DS1820::wait(int timeout)
{
    clockgate_enable(STM32_GPIO_CLOCKGATE(ONEWIRE_PIN.pin >> 4), false);
    idle();
    clockgate_enable(STM32_GPIO_CLOCKGATE(ONEWIRE_PIN.pin >> 4), true);
}

void sensornode_enable_sensors()
{
    clockgate_enable(STM32_GPIO_CLOCKGATE(ONEWIRE_PIN.pin >> 4), true);
}

void sensornode_disable_sensors()
{
    clockgate_enable(STM32_GPIO_CLOCKGATE(ONEWIRE_PIN.pin >> 4), false);
}
