#include "global.h"
#include "sys/serialnum.h"
#include "sys/util.h"


uint32_t __attribute__((pure)) serialnum_chip()
{
    return *((uint32_t*)0x1fff7a10) ^ *((uint32_t*)0x1fff7a14) ^ *((uint32_t*)0x1fff7a18);
}
