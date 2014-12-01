#include "global.h"
#include "sys/time.h"

static uint32_t sys_time_usec_timer_high = 0;
static uint32_t sys_time_usec_timer_last_low = 0;

void __attribute__((optimize("-Os"))) udelay(unsigned int microseconds)
{
    unsigned int timeout = read_usec_timer() + microseconds + 1;
    while ((int)(read_usec_timer() - timeout) < 0);
}

__attribute__((weak,optimize("-Os"))) int64_t read_usec_timer64()
{
    uint32_t low = (uint32_t)read_usec_timer();
    if (low < sys_time_usec_timer_last_low) sys_time_usec_timer_high++;
    sys_time_usec_timer_last_low = low;
    int64_t high = ((int64_t)sys_time_usec_timer_high) << 32;
    return high | low;
}

