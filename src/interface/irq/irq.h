#pragma once

#include "global.h"


#ifdef __cplusplus
extern "C"
{
#endif
    void irq_enable(int irq, bool on);
    void irq_clear_pending(int irq);
#ifdef __cplusplus
}
#endif
