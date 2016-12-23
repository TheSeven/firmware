#pragma once

#include "global.h"


#ifdef __cplusplus
extern "C"
{
#endif
    void clockgate_enable(int gate, bool on);
    bool clockgate_enable_getold(int gate, bool on);
#ifdef __cplusplus
}
#endif
