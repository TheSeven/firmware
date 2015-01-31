#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/spi.h"
#include "device/pcd8544/pcd8544.h"


int main()
{
    PCD8544 lcd(&STM32::SPI::SPI1, PIN_B1, 4000000, PIN_B2, 3, 2, 68);
    lcd.print(0, 0, PCD8544::defaultFont, false, (char*)"Hello, World!");

    while (true);
}
