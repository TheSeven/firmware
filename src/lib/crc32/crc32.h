#pragma once

#include "global.h"


#ifndef CRC32_OPTIMIZE
#define CRC32_OPTIMIZE
#endif


#ifdef __cplusplus
extern "C"
{
#endif
    extern uint32_t crc32(const void* buf, uint32_t len);
#ifdef __cplusplus
}
#endif
