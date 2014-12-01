#pragma once

#include "global.h"
#include "interface/storage/storage.h"


class __attribute__((packed,aligned(4))) SDIO
{
public:
    enum ResponseType
    {
        RESPONSE_NONE = 0,
        RESPONSE_SHORT,
        RESPONSE_LONG,
    };

    struct __attribute__((packed,aligned(4))) Command
    {
        uint32_t cmd : 6;
        ResponseType responseType : 2;
        bool ignoreCRC : 1;
        bool ignoreOpcode : 1;
    };

    enum BlockSize
    {
        BLOCKSIZE_1 = 0x0,
        BLOCKSIZE_2 = 0x1,
        BLOCKSIZE_4 = 0x2,
        BLOCKSIZE_8 = 0x3,
        BLOCKSIZE_16 = 0x4,
        BLOCKSIZE_32 = 0x5,
        BLOCKSIZE_64 = 0x6,
        BLOCKSIZE_128 = 0x7,
        BLOCKSIZE_256 = 0x8,
        BLOCKSIZE_512 = 0x9,
        BLOCKSIZE_1024 = 0xa,
        BLOCKSIZE_2048 = 0xb,
        BLOCKSIZE_4096 = 0xc,
        BLOCKSIZE_8192 = 0xd,
        BLOCKSIZE_16384 = 0xe,
        BLOCKSIZE_STREAM = 0xf,
    };

    enum Timeout
    {
        TIMEOUT_128K = 0x0,
        TIMEOUT_256K = 0x1,
        TIMEOUT_512K = 0x2,
        TIMEOUT_1M = 0x3,
        TIMEOUT_2M = 0x4,
        TIMEOUT_4M = 0x5,
        TIMEOUT_8M = 0x6,
        TIMEOUT_16M = 0x7,
        TIMEOUT_32M = 0x8,
        TIMEOUT_64M = 0x9,
        TIMEOUT_128M = 0xa,
        TIMEOUT_256M = 0xb,
        TIMEOUT_512M = 0xc,
        TIMEOUT_1G = 0xd,
        TIMEOUT_2G = 0xe,
        TIMEOUT_4G = 0xf,
    };

    enum Direction
    {
        DIRECTION_READ = 0,
        DIRECTION_WRITE = 1,
    };

    struct __attribute__((packed,aligned(4))) DataFlags
    {
        enum BlockSize blockSize : 4;
        enum Timeout timeout : 4;
        enum Direction direction : 1;
    };

    const uint8_t maxWidth = 0;
    const bool supportsHighSpeed = false;
    const bool supportsHighCurrent = false;

    constexpr SDIO(int maxWidth, bool supportsHighSpeed, bool supportsHighCurrent)
        : maxWidth(maxWidth), supportsHighSpeed(supportsHighSpeed), supportsHighCurrent(supportsHighCurrent) {}

    virtual enum Storage::Result configure(int frequency, int width) = 0;
    virtual enum Storage::Result command(struct Command command, uint32_t arg, void* response) = 0;
    virtual enum Storage::Result startTransfer(void* buf, uint32_t len, struct DataFlags flags) = 0;
    virtual enum Storage::Result getTransferStatus() = 0;
    virtual enum Storage::Result cancelTransfer() = 0;
};
