#include "global.h"
#include "sys/serialnum.h"
#include "sys/util.h"


uint32_t __attribute__((pure)) serialnum_chip()
{
    return *((uint32_t*)0x1ffff7ac) ^ *((uint32_t*)0x1ffff7b0) ^ *((uint32_t*)0x1ffff7b4);
}
