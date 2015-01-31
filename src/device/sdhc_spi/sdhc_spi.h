#pragma once

#include "global.h"
#include "interface/storage/storage.h"
#include "interface/spi/spi.h"


class __attribute__((packed,aligned(4))) SDHC_SPI final : SPI::Device, public Storage
{
public:
    SDHC_SPI(const SPI::Bus* bus, GPIO::Pin cspin, int maxSpeed);
    virtual enum Result reset();
    virtual enum Result getStatus();
    virtual enum Result read(uint32_t page, uint32_t len, void* buf);
    virtual enum Result write(uint32_t page, uint32_t len, const void* buf);
    virtual enum Result erase(uint32_t page, uint32_t len);

private:
    enum Result readInternal(uint32_t page, uint32_t len, void* buf);
    enum Result writeInternal(uint32_t page, uint32_t len, const void* buf);
    enum Result eraseInternal(uint32_t page, uint32_t len);
    bool waitIdle();
    int sendCmd(uint8_t cmd, uint32_t arg);
    uint32_t readR3();
    bool readBlock(void* buf, int len);
    bool writeBlock(const void* buf, int len, uint8_t token);
    int maxSpeed;
    bool initialized;
    bool sdhc;
};
