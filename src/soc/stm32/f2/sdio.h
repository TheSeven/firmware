#pragma once

#include "global.h"
#include "interface/sdio/sdio.h"


namespace STM32
{

    class __attribute__((packed,aligned(4))) SDIO final : public ::SDIO
    {
    public:
        constexpr SDIO(int maxWidth, bool supportsHighSpeed, bool supportsHighCurrent)
            : ::SDIO(maxWidth, supportsHighSpeed, supportsHighCurrent) {}

        enum Storage::Result configure(int frequency, int width);
        enum Storage::Result command(struct Command command, uint32_t arg, void* response);
        enum Storage::Result startTransfer(void* buf, uint32_t len, struct DataFlags flags);
        enum Storage::Result getTransferStatus();
        enum Storage::Result cancelTransfer();

    private:
        uint32_t* buffer = NULL;
        uint32_t length = 0;
        struct DataFlags dataFlags = {};
    };

}
