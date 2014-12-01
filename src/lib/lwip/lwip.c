#include "global.h"
#include "lib/lwip/lwip.h"
#include "sys/time.h"

u32_t sys_now()
{
    return read_usec_timer64() >> 10;
}
