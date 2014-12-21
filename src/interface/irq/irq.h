#pragma once

#include "global.h"


#ifdef __cplusplus
extern "C"
{
#endif
    void irq_enable(int irq, bool on);
    bool irq_get_pending(int irq);
    void irq_clear_pending(int irq);
    void irq_set_pending(int irq);
    void irq_set_priority(int irq, int priority);
    void irq_set_priority_grouping(int bits);
#ifdef __cplusplus
}
#endif
