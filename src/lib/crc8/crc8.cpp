#include "global.h"
#include "lib/crc8/crc8.h"


__attribute__((weak)) uint8_t CRC8_OPTIMIZE crc8(const void* buf, uint32_t len)
{
    uint8_t* data = (uint8_t*)buf;
    uint8_t crc = 0;
    while (len--)
    {
        uint8_t byte = *data++;
        crc = crc ^ byte;
        for (int i = 7; i >= 0; i--) crc = (crc >> 1) ^ (0x8c & (-(crc & 1)));
    }
    return crc;
}
