#include "global.h"
#include "lib/crc32/crc32.h"
#include "soc/stm32/crc32_regs.h"
#include "interface/clockgate/clockgate.h"
#include "sys/util.h"


uint32_t crc32(const void* buf, uint32_t len)
{
    uint32_t* data = (uint32_t*)buf;
    clockgate_enable(STM32_CRC_CLOCKGATE, true);
    union STM32_CRC_REG_TYPE::CR CR = { 0 };
    CR.b.RESET = true;
    STM32_CRC_REGS.CR.d32 = CR.d32;
    while (len >= 1)
    {
        STM32_CRC_REGS.DR = *data++;
        len -= 4;
    }
    uint32_t crc = STM32_CRC_REGS.DR;
    clockgate_enable(STM32_CRC_CLOCKGATE, false);
    return crc;
}
