#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/spi.h"
#include "device/pcd8544/pcd8544.h"


PCD8544 lcd(&STM32::SPI::SPI1, PIN_B12, 4000000, PIN_B2, 3, 2, 68);


int main()
{
    lcd.init();
    lcd.print(0, 0, lcd.defaultFont, false, "Hello, World!");
    GPIO::configure(PIN_B8, GPIO::MODE_OUTPUT, false);

    while (true);
}
