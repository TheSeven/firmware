#pragma once

#include "global.h"
#include "cpu/arm/cortexm/irq.h"

#ifdef CPU_ARM_CORTEX_M0
#include "lib/cmsis/core_cm0.h"
#endif
#ifdef CPU_ARM_CORTEX_M0p
#include "lib/cmsis/core_cm0plus.h"
#endif
#ifdef CPU_ARM_CORTEX_M3
#include "lib/cmsis/core_cm3.h"
#endif
#ifdef CPU_ARM_CORTEX_M4
#include "lib/cmsis/core_cm4.h"
#endif

