#include "global.h"
#include "lib/lwip/lwip.h"
#include "sys/util.h"
#include "sys/time.h"


sys_prot_t sys_arch_protect()
{
    sys_prot_t old = get_critsec_state();
    if (!old) enter_critical_section();
    return old;
}

void sys_arch_unprotect(sys_prot_t old)
{
    if (!old) leave_critical_section();
}

u32_t sys_now()
{
    return read_usec_timer64() >> 10;
}
