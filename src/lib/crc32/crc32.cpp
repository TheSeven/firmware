#include "global.h"
#include "lib/crc32/crc32.h"


__attribute__((weak)) uint32_t CRC32_OPTIMIZE crc32(const void* buf, uint32_t len)
{
    uint8_t* data = (uint8_t*)buf;
    uint32_t crc = 0xffffffff;
    while (len--)
    {
        uint8_t byte = *data++;
        crc = crc ^ byte;
        for (int i = 7; i >= 0; i--) crc = (crc >> 1) ^ (0xedb88320 & (-(crc & 1)));
    }
    return ~crc;
}
