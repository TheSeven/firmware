#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/spi.h"


int main()
{
    while (true)
    {
        GPIO::setLevel(PIN_A4, true);
        udelay(200000);
        GPIO::setLevel(PIN_A4, false);
        udelay(200000);
    }
}
