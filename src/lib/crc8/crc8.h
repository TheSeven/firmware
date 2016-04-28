#pragma once

#include "global.h"


#ifndef CRC8_OPTIMIZE
#define CRC8_OPTIMIZE
#endif


#ifdef __cplusplus
extern "C"
{
#endif
    extern uint8_t crc8(const void* buf, uint32_t len);
#ifdef __cplusplus
}
#endif
