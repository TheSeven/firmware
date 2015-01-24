#pragma once

#include "global.h"

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define CONCAT2(x,y) x#y
#define CONCAT(x,y) CONCAT2(x,y)

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))
#define ABS(a) (((a)>0)?(a):-(a))
#define BIT(x) (1 << (x))
#define BITRANGE(x, y) ((0xfffffffful >> (31 + (x) - (y))) << (x))
#define ARRAYLEN(x) (sizeof(x) ? sizeof(x) / sizeof(*(x)) : 0)
#define PUN_PTR(type, p) ((type)(intptr_t)(p))
#define OFFSETOF(type, membername) ((off_t)&((type *)0)->membername)
#define SKIPBYTES(p, count) ((typeof (p))((char *)(p) + (count)))
#define ALIGN_UP_DIV(x, y) (((x) + (y) - 1) / (y))
#define ALIGN_UP(x, y) ((typeof(x))ALIGN_UP_DIV((uint32_t)x, y) * (y))

#define SWAP_16(x) ((typeof(x))(unsigned short)(((unsigned short)(x) >> 8) | ((unsigned short)(x) << 8)))
#define SWAP_32(x) ((typeof(x))(unsigned long)((((unsigned long)(x) & 0xff000000ul) >> 24) | \
                                                  (((unsigned long)(x) & 0xff0000ul) >> 8) | \
                                                  (((unsigned long)(x) & 0xff00ul) << 8) | \
                                                  (((unsigned long)(x) & 0xfful) << 24)))
#define REVERSE_32(x) ((typeof(x))(unsigned long)((((unsigned long)(x) & 0x80000000ul) >> 31) | \
                                                     (((unsigned long)(x) & 0x40000000ul) >> 29) | \
                                                     (((unsigned long)(x) & 0x20000000ul) >> 27) | \
                                                     (((unsigned long)(x) & 0x10000000ul) >> 25) | \
                                                     (((unsigned long)(x) & 0x8000000ul) >> 23) | \
                                                     (((unsigned long)(x) & 0x4000000ul) >> 21) | \
                                                     (((unsigned long)(x) & 0x2000000ul) >> 19) | \
                                                     (((unsigned long)(x) & 0x1000000ul) >> 17) | \
                                                     (((unsigned long)(x) & 0x800000ul) >> 15) | \
                                                     (((unsigned long)(x) & 0x400000ul) >> 13) | \
                                                     (((unsigned long)(x) & 0x200000ul) >> 11) | \
                                                     (((unsigned long)(x) & 0x100000ul) >> 9) | \
                                                     (((unsigned long)(x) & 0x80000ul) >> 7) | \
                                                     (((unsigned long)(x) & 0x40000ul) >> 5) | \
                                                     (((unsigned long)(x) & 0x20000ul) >> 3) | \
                                                     (((unsigned long)(x) & 0x10000ul) >> 1) | \
                                                     (((unsigned long)(x) & 0x8000ul) << 1) | \
                                                     (((unsigned long)(x) & 0x4000ul) << 3) | \
                                                     (((unsigned long)(x) & 0x2000ul) << 5) | \
                                                     (((unsigned long)(x) & 0x1000ul) << 7) | \
                                                     (((unsigned long)(x) & 0x800ul) << 9) | \
                                                     (((unsigned long)(x) & 0x400ul) << 11) | \
                                                     (((unsigned long)(x) & 0x200ul) << 13) | \
                                                     (((unsigned long)(x) & 0x100ul) << 15) | \
                                                     (((unsigned long)(x) & 0x80ul) << 17) | \
                                                     (((unsigned long)(x) & 0x40ul) << 19) | \
                                                     (((unsigned long)(x) & 0x20ul) << 21) | \
                                                     (((unsigned long)(x) & 0x10ul) << 23) | \
                                                     (((unsigned long)(x) & 0x8ul) << 25) | \
                                                     (((unsigned long)(x) & 0x4ul) << 27) | \
                                                     (((unsigned long)(x) & 0x2ul) << 29) | \
                                                     (((unsigned long)(x) & 0x1ul) << 31)))
#ifdef ENDIANNESS_LITTLE
#define LE_TO_H16(x) (x)
#define LE_TO_H32(x) (x)
#define H_TO_LE16(x) (x)
#define H_TO_LE32(x) (x)
#define BE_TO_H16(x) SWAP_16(x)
#define BE_TO_H32(x) SWAP_32(x)
#define H_TO_BE16(x) SWAP_16(x)
#define H_TO_BE32(x) SWAP_32(x)
#define letoh16(x) (x)
#define letoh32(x) (x)
#define htole16(x) (x)
#define htole32(x) (x)
#define betoh16(x) swap16(x)
#define betoh32(x) swap32(x)
#define htobe16(x) swap16(x)
#define htobe32(x) swap32(x)
#else
#define LE_TO_H16(x) SWAP_16(x)
#define LE_TO_H32(x) SWAP_32(x)
#define H_TO_LE16(x) SWAP_16(x)
#define H_TO_LE32(x) SWAP_32(x)
#define BE_TO_H16(x) (x)
#define BE_TO_H32(x) (x)
#define H_TO_BE16(x) (x)
#define H_TO_BE32(x) (x)
#define letoh16(x) swap16(x)
#define letoh32(x) swap32(x)
#define htole16(x) swap16(x)
#define htole32(x) swap32(x)
#define betoh16(x) (x)
#define betoh32(x) (x)
#define htobe16(x) (x)
#define htobe32(x) (x)
#endif

#ifndef CACHEALIGN_BITS
#define CACHEALIGN_BITS 2
#endif
#define CACHEALIGN_SIZE BIT(CACHEALIGN_BITS)
#define CACHEALIGN_ATTR __attribute__((aligned(CACHEALIGN_SIZE)))
#define CACHEALIGN_AT_LEAST_ATTR(x) __attribute__((aligned(MAX(CACHEALIGN_SIZE, (x)))))
#ifndef DMAALIGN_BITS
#define DMAALIGN_BITS 2
#endif
#define DMAALIGN_SIZE BIT(DMAALIGN_BITS)
#define DMAALIGN_ATTR __attribute__((aligned(DMAALIGN_SIZE)))
#define DMAALIGN_AT_LEAST_ATTR(x) __attribute__((aligned(MAX(DMAALIGN_SIZE, (x)))))


#ifndef ASM_FILE
#ifdef __cplusplus
extern "C"
{
#endif
    __attribute__((noreturn)) void powerdown();
    __attribute__((noreturn)) void reset();
    __attribute__((noreturn)) void hang();
    void idle();
    void __cxa_pure_virtual();
    uint64_t __aeabi_ldiv0();
    uint32_t __aeabi_idiv0();
    __attribute__((noreturn)) void execfirmware(void* address);
    void enter_critical_section();
    void leave_critical_section();
    void clean_dcache(const void* addr, uint32_t len);
    void invalidate_dcache(const void* addr, uint32_t len);
    void invalidate_icache(const void* addr, uint32_t len);
    void enable_mmu();
    void disable_mmu();
    __attribute__((const)) uint32_t swap32(uint32_t data);
    __attribute__((const)) uint32_t swap16(uint32_t data);
    __attribute__((const)) uint32_t reverse32(uint32_t data);
    void* memcpy(void* dst, const void* src, size_t len);
    void* memmove(void* dst, const void* src, size_t len);
    void* memset(void* dst, int val, size_t len);
    int memcmp(const void* a, const void* b, size_t len);
    size_t strlen(const char* string);
    void to_hex(char* dest, int len, uint32_t value);
#ifdef __cplusplus
}
#endif

static inline void discard(uint32_t data)
{
    (void)data;
}

#ifdef __cplusplus
inline void * operator new (size_t, void * p) throw() { return p ; }
inline void * operator new[] (size_t, void * p) throw() { return p ; }
inline void operator delete (void *, void *) throw() {}
inline void operator delete[] (void *, void *) throw() {}
#endif

#endif
