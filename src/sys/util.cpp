#include "global.h"
#include "sys/util.h"

__attribute__((pure,weak)) void idle()
{
}

__attribute__((noreturn,weak)) void hang()
{
    enter_critical_section();
    while (true) idle();
}

__attribute__((noreturn,weak,alias("hang"))) void powerdown();

extern "C" __attribute__((weak,alias("hang"))) void __cxa_pure_virtual();

extern "C" __attribute__((weak)) uint64_t __aeabi_ldiv0()
{
    return 0;
}

extern "C" __attribute__((weak,alias("__aeabi_ldiv0"))) uint32_t __aeabi_idiv0();

__attribute__((noreturn,weak)) void execfirmware(void* address)
{
    __attribute__((noreturn)) void (*firmware)() = (TYPE_NORETURN(*)())address;
    enter_critical_section();
    clean_dcache(0, 0xffffffff);
    invalidate_icache(0, 0xffffffff);
    disable_mmu();
    firmware();
}

__attribute__((weak)) void enter_critical_section()
{
}

__attribute__((weak)) void leave_critical_section()
{
}

__attribute__((weak)) void clean_dcache(const void* addr, uint32_t len)
{
}

__attribute__((weak)) void invalidate_dcache(const void* addr, uint32_t len)
{
}

__attribute__((weak)) void invalidate_icache(const void* addr, uint32_t len)
{
}

__attribute__((weak)) void enable_mmu()
{
}

__attribute__((weak)) void disable_mmu()
{
}

__attribute__((const,weak)) uint32_t swap32(uint32_t data)
{
    return SWAP_32(data);
}

__attribute__((const,weak)) uint32_t swap16(uint32_t data)
{
    return SWAP_16(data);
}

__attribute__((const,weak)) uint32_t reverse32(uint32_t data)
{
    return REVERSE_32(data);
}

#define UNALIGNED(x) (((long)x & (sizeof (long) - 1)))
#define UNALIGNED2(x, y) (((long)x & (sizeof (long) - 1)) | ((long)y & (sizeof (long) - 1)))
#define BIGBLOCKSIZE (sizeof(long) << 2)
#define LITTLEBLOCKSIZE (sizeof(long))
#define TOO_SMALL(LEN) ((LEN) < BIGBLOCKSIZE)

__attribute__((weak)) void* memset(void* dst_void, int val, size_t len)
{
    val &= 0xff;
    uint8_t* dst = (uint8_t*)dst_void;
#ifndef SQUEEZE
    if (!TOO_SMALL(len) && !UNALIGNED(dst))
    {
        unsigned long* aligned_dst = (unsigned long*)dst;
        unsigned long longval = (val << 8) | val;
        longval |= longval << 16;
        while (len >= BIGBLOCKSIZE)
        {
            *aligned_dst++ = longval;
            *aligned_dst++ = longval;
            *aligned_dst++ = longval;
            *aligned_dst++ = longval;
            len -= BIGBLOCKSIZE;
        }
        while (len >= LITTLEBLOCKSIZE)
        {
            *aligned_dst++ = longval;
            len -= LITTLEBLOCKSIZE;
        }
        dst = (uint8_t*)aligned_dst;
      }
#endif
    while (len--) *dst++ = val;
    return dst_void;
}

__attribute__((weak)) void* memmove(void* dst_void, const void* src_void, size_t len)
{
    char* dst = (char*)dst_void;
    const char* src = (char*)src_void;
    if (src < dst && dst < src + len)
    {
        src += len;
        dst += len;
        while (len--) *--dst = *--src;
    }
    else
    {
#ifndef SQUEEZE
        if (!TOO_SMALL(len) && !UNALIGNED2(src, dst))
        {
            long* aligned_dst = (long*)dst;
            const long* aligned_src = (long*)src;
            while (len >= BIGBLOCKSIZE)
            {
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                len -= BIGBLOCKSIZE;
            }
            while (len >= LITTLEBLOCKSIZE)
            {
                *aligned_dst++ = *aligned_src++;
                len -= LITTLEBLOCKSIZE;
            }
            dst = (char*)aligned_dst;
            src = (const char*)aligned_src;
        }
#endif
        while (len--) *dst++ = *src++;
    }
    return dst_void;
}

__attribute__((weak,alias("memmove"))) void* memcpy(void* dst, const void* src, size_t len);

__attribute__((weak)) int memcmp(const void* ptr1, const void* ptr2, size_t len)
{
    uint8_t* a = (uint8_t*)ptr1;
    uint8_t* b = (uint8_t*)ptr2;
    int d;
    while (len--)
        if ((d = *a++ - *b++))
            return d;
    return 0;
}

__attribute__((weak)) size_t strlen(const char* string)
{
    const char* pos = string;
    while (*pos++);
    return pos - string - 1;
}

static const char hextab[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void to_hex(char* dest, int len, uint32_t value)
{
    dest += len;
    while (len--)
    {
        *--dest = hextab[value & 0xf];
        value >>= 4;
    }
}
