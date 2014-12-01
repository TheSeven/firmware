#include "global.h"
#include "sys/serialnum.h"

uint32_t __attribute__((weak,pure)) serialnum_chip()
{
    return 0xffffffff;
}

uint32_t __attribute__((weak)) serialnum_board()
{
    return serialnum_chip();
}

