#include "global.h"
#include "lcd.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/spi.h"
#include "sys/util.h"
#include "font.h"
#include "stepper.h"


PCD8544 lcd(&STM32::SPI::SPI1, PIN_F0, 4000000, PIN_F1, 3, 2, 68);


void drawStatus()
{
    lcd.print(0, 0, font, 0x80, "StepperTester 1.0");
    lcd.print(1, 39, font, 0, "by Michael");
    lcd.print(2, 44, font, 0, "Sparmann");
    lcd.printf(3, 7, font, 0, "Target: %+8d", targetpos);
    lcd.printf(4, 4, font, 0, "Position: %+8d", pos);
    lcd.printf(5, 13, font, 0, "Speed: %+8d", speed * dir);
}
