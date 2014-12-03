#include "global.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/uart.h"

/*    STM32::UART::Config uartConfig = { };
    uartConfig.stopBits = uartConfig.StopBits2_0;
    STM32::UART::UART1.configure(250000, 1000, uartConfig);
*/
//    int i = 0;
/*        STM32::UART::UART1.flush();
        GPIO::configure(PIN_A9, GPIO::MODE_OUTPUT, false);
        udelay(88);
        GPIO::setMode(PIN_A9, GPIO::MODE_SPECIAL);
        udelay(8);
        STM32::UART::UART1.bwTx(0);
        STM32::UART::UART1.bwTx(MIN(255, ABS(256 - i)));
        STM32::UART::UART1.bwTx(MIN(255, ABS(512 - i)));
        STM32::UART::UART1.bwTx(MIN(255, ABS(768 - i)));
        STM32::UART::UART1.bwTx(0);
        STM32::UART::UART1.bwTx(0);
        STM32::UART::UART1.bwTx(0);
        i++;
        if (i >= 1024) i = 0;
*/

void initDMX()
{
}

void pollDMX()
{
}
