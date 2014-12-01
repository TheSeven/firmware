#pragma once

#ifndef ASM_FILE
#ifdef __clang__
#define TYPE_NORETURN void __attribute__((noreturn))
typedef unsigned __INT8_TYPE__ uint8_t;
typedef unsigned __INT16_TYPE__ uint16_t;
typedef unsigned __INT32_TYPE__ uint32_t;
typedef unsigned __INT64_TYPE__ uint64_t;
typedef unsigned __INTPTR_TYPE__ uintptr_t;
#else
#define TYPE_NORETURN void
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __UINTPTR_TYPE__ uintptr_t;
#endif
typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
typedef __SIZE_TYPE__ size_t;
typedef __INTPTR_TYPE__ intptr_t;
#ifndef __cplusplus
typedef uint8_t bool;
#define true 1
#define false 0
#endif
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

#include "config.h"

