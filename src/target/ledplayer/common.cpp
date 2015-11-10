#include "common.h"

#include "global.h"
#include "soc/stm32/spi.h"
#include "soc/stm32/gpio.h"


SDHC_SPI sd(&STM32::SPI::SPI1, PIN_A8, 25000000);
FatFs::FileSystem fs(&sd, 0, "0:", NULL);

PCD8544 lcd(&STM32::SPI::SPI1, PIN_B12, 4000000, PIN_B2, 3, 2, 68);
