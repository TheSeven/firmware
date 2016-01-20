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
#ifdef SPIFLASH_SUPPORT_MX25L400
    { .jedecId = { 0xc2, 0x20, 0x13 }, .eraseCmd = 0x20, .maxSpeed = 70, .pageSize = 8, .sectorSize = 4, .sectorCount = 7 },
#endif
#ifdef SPIFLASH_SUPPORT_PM25W20
    { .jedecId = { 0x7f, 0x9d, 0x7d }, .eraseCmd = 0x20, .maxSpeed = 50, .pageSize = 8, .sectorSize = 4, .sectorCount = 10 },
#endif
};

SPIFLASH_OPTIMIZE SPIFlash::SPIFlash(const SPI::Bus* bus, GPIO::Pin cspin, int initSpeed, int maxSpeed)
    : Device(bus, cspin, initSpeed), initSpeed(initSpeed), maxSpeed(maxSpeed), sleeping(true)
{
    reset();
}

bool SPIFLASH_OPTIMIZE SPIFlash::waitIdle(bool oldAwake)
{
    long timeout = TIMEOUT_SETUP(2000000);
    while (!TIMEOUT_EXPIRED(timeout))
    {
        pushByte(0x05);
        uint8_t status = pushByte(0);
        reselect();
        if (!(status & 1)) return true;
    }
    deselect();
    stayAwake(oldAwake);
    return false;
}

bool SPIFLASH_OPTIMIZE SPIFlash::enableWrite(bool oldAwake)
{
    pushByte(0x06);
    reselect();
    pushByte(0x05);
    uint8_t status = pushByte(0);
    reselect();
    if (status & 2) return true;
    deselect();
    stayAwake(oldAwake);
    return false;
}

enum Storage::Result SPIFLASH_OPTIMIZE SPIFlash::reset()
{
    initialized = false;
    frequency = initSpeed;
    bool oldAwake = stayAwake(true);
    select();
    if (!waitIdle(oldAwake)) return RESULT_COMM_ERROR;
    pushByte(0x9f);
    uint8_t manufacturer = pushByte(0);
    uint8_t type = pushByte(0);
    uint8_t density = pushByte(0);
    if (manufacturer == type && manufacturer == density)
    {
        reselect();
        pushByte(0x15);
        manufacturer = pushByte(0);
        type = pushByte(0);
        density = 0xff;
        if (manufacturer == type)
        {
            deselect();
            stayAwake(oldAwake);
            return RESULT_UNSUPPORTED;
        }
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
    if (!deviceType)
    {
        deselect();
        stayAwake(oldAwake);
        return RESULT_UNSUPPORTED;
    }
    pageCount = 1 << (deviceType->sectorCount + deviceType->sectorSize + deviceType->pageSize);
    if (!pageCount) pageCount = 0xffffffff;
    pageSize = 1;
    programSize = 1;
    eraseSize = 1 << (deviceType->sectorSize + deviceType->pageSize);
    eraseCmd = deviceType->eraseCmd;
    frequency = MIN(maxSpeed, deviceType->maxSpeed << 20);
    reselect();
    pushByte(0x50);
    reselect();
    pushByte(0x01);
    pushByte(0x00);
    deselect();
    stayAwake(oldAwake);
    initialized = true;
    return RESULT_OK;
}

enum Storage::Result SPIFLASH_OPTIMIZE SPIFlash::getStatus()
{
    return initialized ? RESULT_OK : RESULT_INVALID_STATE;
}

enum Storage::Result SPIFLASH_OPTIMIZE SPIFlash::read(uint32_t page, uint32_t len, void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    bool oldAwake = stayAwake(true);
    select();
    if (!waitIdle(oldAwake)) return RESULT_COMM_ERROR;
    pushByte(0x03);
    pushByte(page >> 16);
    pushByte(page >> 8);
    pushByte(page);
    pushBuffer(NULL, buf, len);
    deselect();
    stayAwake(oldAwake);
    return RESULT_OK;
}

enum Storage::Result SPIFLASH_OPTIMIZE SPIFlash::write(uint32_t page, uint32_t len, const void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    bool oldAwake = stayAwake(true);
    select();
    while (len)
    {
        int chunklen = MIN(pageSize - (page & (pageSize - 1)), len);
        if (!waitIdle(oldAwake)) return RESULT_COMM_ERROR;
        if (!enableWrite(oldAwake)) return RESULT_WRITE_PROTECTED;
        pushByte(0x02);
        pushByte(page >> 16);
        pushByte(page >> 8);
        pushByte(page);
        pushBuffer(buf, NULL, chunklen);
        buf = (void*)(((uintptr_t)buf) + chunklen);
        page += chunklen;
        len -= chunklen;
        if (len) reselect();
    }
    deselect();
    stayAwake(oldAwake);
    return RESULT_OK;
}

enum Storage::Result SPIFLASH_OPTIMIZE SPIFlash::erase(uint32_t page, uint32_t len)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (page & (eraseSize - 1)) return RESULT_INVALID_ARGUMENT;
    if (len & (eraseSize - 1)) return RESULT_INVALID_ARGUMENT;
    bool oldAwake = stayAwake(true);
    select();
    while (len)
    {
        if (!waitIdle(oldAwake)) return RESULT_COMM_ERROR;
        if (!enableWrite(oldAwake)) return RESULT_WRITE_PROTECTED;
        pushByte(eraseCmd);
        pushByte(page >> 16);
        pushByte(page >> 8);
        pushByte(page);
        page += eraseSize;
        len -= eraseSize;
        if (len) reselect();
    }
    deselect();
    stayAwake(oldAwake);
    return RESULT_OK;
}

bool SPIFLASH_OPTIMIZE SPIFlash::stayAwake(bool on)
{
    if (sleeping == !on) return on;
    if (!on)
    {
        select();
        pushByte(0xb9);
        deselect();
    }
    SPI::Device::stayAwake(on);
    if (on)
    {
        select();
        pushByte(0xab);
        deselect();
        udelay(3);
    }
    sleeping = !on;
    return !on;
}
