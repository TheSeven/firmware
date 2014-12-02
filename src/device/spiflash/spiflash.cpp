#include "global.h"
#include "device/spiflash/spiflash.h"
#include "sys/time.h"
#include "sys/util.h"


const struct SPIFlash::DeviceType SPIFlash::deviceTypes[] =
{
#ifdef SPIFLASH_SUPPORT_AT25F512
    { .jedecId = { 0x1f, 0x65, 0xff }, .eraseCmd = 0x52, .maxSpeed = 20, .pageSize = 8, .sectorSize = 7, .sectorCount = 1 },
#endif
#ifdef SPIFLASH_SUPPORT_AT25F1024
    { .jedecId = { 0x1f, 0x60, 0xff }, .eraseCmd = 0x52, .maxSpeed = 20, .pageSize = 8, .sectorSize = 7, .sectorCount = 2 },
#endif
#ifdef SPIFLASH_SUPPORT_W25Q80BV
    { .jedecId = { 0xef, 0x40, 0x14 }, .eraseCmd = 0x20, .maxSpeed = 104, .pageSize = 8, .sectorSize = 4, .sectorCount = 8 },
#endif
};

SPIFlash::SPIFlash(const SPI::Bus* bus, GPIO::Pin cspin, int initSpeed, int maxSpeed)
    : Device(bus, cspin, initSpeed), initSpeed(initSpeed), maxSpeed(maxSpeed)
{
    reset();
}

bool SPIFlash::waitIdle()
{
    long timeout = TIMEOUT_SETUP(2000000);
    while (!TIMEOUT_EXPIRED(timeout))
    {
        select();
        pushByte(0x05);
        uint8_t result = pushByte(0);
        deselect();
        if (!(result & 1)) return true;
    }
    return false;
}

enum Storage::Result SPIFlash::reset()
{
    initialized = false;
    frequency = initSpeed;
    if (!waitIdle()) return RESULT_COMM_ERROR;
    select();
    pushByte(0x9f);
    uint8_t manufacturer = pushByte(0);
    uint8_t type = pushByte(0);
    uint8_t density = pushByte(0);
    deselect();
    if (manufacturer == type && manufacturer == density)
    {
        select();
        pushByte(0x15);
        manufacturer = pushByte(0);
        type = pushByte(0);
        density = 0xff;
        deselect();
        if (manufacturer == type) return RESULT_UNSUPPORTED;
    }
    const DeviceType* deviceType = NULL;
    for (uint32_t i = 0; i < ARRAYLEN(deviceTypes); i++)
    {
        if (deviceTypes[i].jedecId.manufacturer != manufacturer) continue;
        if (deviceTypes[i].jedecId.type != type) continue;
        if (deviceTypes[i].jedecId.density != density) continue;
        deviceType = &deviceTypes[i];
        break;
    }
    if (!deviceType) return RESULT_UNSUPPORTED;
    pageCount = 1 << (deviceType->sectorCount + deviceType->sectorSize + deviceType->pageSize);
    if (!pageCount) pageCount = 0xffffffff;
    pageSize = 1;
    programSize = 1;
    eraseSize = 1 << (deviceType->sectorSize + deviceType->pageSize);
    eraseCmd = deviceType->eraseCmd;
    frequency = MIN(maxSpeed, deviceType->maxSpeed << 10);
    select();
    pushByte(0x50);
    deselect();
    select();
    pushByte(0x01);
    pushByte(0x00);
    deselect();
    initialized = true;
    return RESULT_OK;
}

enum Storage::Result SPIFlash::getStatus()
{
    return initialized ? RESULT_OK : RESULT_INVALID_STATE;
}

enum Storage::Result SPIFlash::read(uint32_t page, uint32_t len, void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    if (!waitIdle()) return RESULT_COMM_ERROR;
    select();
    pushByte(0x03);
    pushByte(page >> 16);
    pushByte(page >> 8);
    pushByte(page);
    pushBuffer(NULL, buf, len);
    deselect();
    return RESULT_OK;
}

enum Storage::Result SPIFlash::write(uint32_t page, uint32_t len, const void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    while (len)
    {
        int chunklen = MIN(pageSize - (page & (pageSize - 1)), len);
        if (!waitIdle()) return RESULT_COMM_ERROR;
        select();
        pushByte(0x06);
        deselect();
        select();
        pushByte(0x02);
        pushByte(page >> 16);
        pushByte(page >> 8);
        pushByte(page);
        pushBuffer(buf, NULL, chunklen);
        deselect();
        buf = (void*)(((uintptr_t)buf) + chunklen);
        page += chunklen;
        len -= chunklen;
    }
    return RESULT_OK;
}

enum Storage::Result SPIFlash::erase(uint32_t page, uint32_t len)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (page & (eraseSize - 1)) return RESULT_INVALID_ARGUMENT;
    if (len & (eraseSize - 1)) return RESULT_INVALID_ARGUMENT;
    while (len)
    {
        if (!waitIdle()) return RESULT_COMM_ERROR;
        select();
        pushByte(0x06);
        deselect();
        select();
        pushByte(eraseCmd);
        pushByte(page >> 16);
        pushByte(page >> 8);
        pushByte(page);
        deselect();
        page += eraseSize;
        len -= eraseSize;
    }
    return RESULT_OK;
}
