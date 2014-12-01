#pragma once

#include "global.h"
#include "interface/storage/storage.h"
#include "interface/sdio/sdio.h"


class __attribute__((packed,aligned(4))) SDHC_SDIO final : public Storage
{
public:
    SDHC_SDIO(SDIO* sdio, int maxSpeed, int retries);
    virtual enum Result reset();
    virtual enum Result getStatus();
    virtual enum Result read(uint32_t page, uint32_t len, void* buf);
    virtual enum Result write(uint32_t page, uint32_t len, const void* buf);
    virtual enum Result erase(uint32_t page, uint32_t len);

private:
    enum Result checkResponse(uint32_t response);
    enum Result prepareACMD();
    enum Result acmd(struct SDIO::Command command, uint32_t arg, void* response);
    enum Result modeSwitch(uint32_t argument, void* buf);
    SDIO* sdio;
    int maxSpeed;
    uint16_t rca;
    uint8_t retries;
    bool initialized : 1;
    bool sdhc : 1;
    int reserved : 6;
};
