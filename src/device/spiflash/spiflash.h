#pragma once

#include "global.h"
#include "interface/storage/storage.h"
#include "interface/spi/spi.h"


#ifndef SPIFLASH_OPTIMIZE
#define SPIFLASH_OPTIMIZE
#endif


class __attribute__((packed,aligned(4))) SPIFlash final : SPI::Device, public Storage
{
public:
    struct __attribute__((packed,aligned(4))) DeviceType
    {
        struct __attribute__((packed))
        {
            uint8_t manufacturer;
            uint8_t type;
            uint8_t density;
        } jedecId;
        uint8_t eraseCmd;
        uint8_t maxSpeed;
        uint8_t pageSize;
        uint8_t sectorSize;
        uint8_t sectorCount;
    };

    static const struct DeviceType deviceTypes[];

    SPIFlash(const SPI::Bus* bus, GPIO::Pin cspin, int initSpeed, int maxSpeed);
    virtual enum Result reset();
    virtual enum Result getStatus();
    virtual enum Result read(uint32_t page, uint32_t len, void* buf);
    virtual enum Result write(uint32_t page, uint32_t len, const void* buf);
    virtual enum Result erase(uint32_t page, uint32_t len);

private:
    bool waitIdle();
    int initSpeed;
    int maxSpeed;
    uint8_t eraseCmd;
    bool initialized;
};
