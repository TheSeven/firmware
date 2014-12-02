#include "global.h"
#include "sys/serialnum.h"
#include "sys/util.h"


uint32_t __attribute__((pure)) serialnum_chip()
{
    return *((uint32_t*)0x1ffff7e8) ^ *((uint32_t*)0x1ffff7ec) ^ *((uint32_t*)0x1ffff7f0);
}
