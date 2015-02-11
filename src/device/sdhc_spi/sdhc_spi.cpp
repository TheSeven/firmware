#include "global.h"
#include "device/sdhc_spi/sdhc_spi.h"
#include "sys/time.h"
#include "sys/util.h"


SDHC_SPI::SDHC_SPI(const SPI::Bus* bus, GPIO::Pin cspin, int maxSpeed)
    : Device(bus, cspin, maxSpeed), maxSpeed(maxSpeed)
{
    pageSize = 512;
    programSize = 1;
    eraseSize = 0;
    reset();
}

bool SDHC_SPI::waitIdle()
{
    long timeout = TIMEOUT_SETUP(500000);
    uint8_t data = 0;
    while (!TIMEOUT_EXPIRED(timeout) && data != 0xff) data = pushByte(0xff);
    if (data != 0xff) return false;
    return true;
}

int SDHC_SPI::sendCmd(uint8_t cmd, uint32_t arg)
{
    if (cmd & 0x80)
    {
        cmd &= 0x7f;
        int rc = sendCmd(55, 0);
        if (rc < 0) return rc;
    }

    if (!waitIdle()) return -1;

    pushByte(0x40 | cmd);
    pushByte(arg >> 24);
    pushByte(arg >> 16);
    pushByte(arg >> 8);
    pushByte(arg);
    uint8_t crcbyte = 1;
    if (cmd == 0) crcbyte = 0x95;
    else if (cmd == 8 && arg == 0x1aa) crcbyte = 0x87;
    pushByte(crcbyte);

    int tries = 10;
    if (cmd == 12) pushByte(0xff);
    udelay(1);
    uint8_t data = 0xff;
    while (tries-- && (data & 0x80)) data = pushByte(0xff);
    if (data & 0x80) return -2;

    return data;
}

uint32_t SDHC_SPI::readR3()
{
    int bytes = 4;
    uint32_t data = 0;
    while (bytes--) data = (data << 8) | pushByte(0xff);
    return data;
}

bool SDHC_SPI::readBlock(void* buf, int len)
{
    udelay(1);
    uint8_t data = 0xff;
    long timeout = TIMEOUT_SETUP(100000);
    while (!TIMEOUT_EXPIRED(timeout) && data == 0xff) data = pushByte(0xff);
    if (data != 0xfe) return false;
    pushBuffer(NULL, buf, len);
    pushByte(0xff);
    pushByte(0xff);
    return true;
}

bool SDHC_SPI::writeBlock(const void* buf, int len, uint8_t token)
{
    uint8_t data = 0;
    long timeout = TIMEOUT_SETUP(500000);
    while (!TIMEOUT_EXPIRED(timeout) && data != 0xff) data = pushByte(0xff);
    if (data != 0xff) return false;
    pushByte(token);
    if (len) pushBuffer(buf, NULL, len);
    pushByte(0xff);
    pushByte(0xff);
    udelay(1);
    if ((pushByte(0xff) & 0x1f) != 5) return false;
    return true;
}

enum Storage::Result SDHC_SPI::reset()
{
    initialized = false;
    sdhc = false;
    pageCount = 0;
    deselbytes = 10;
    frequency = 400000;
    select();
    deselect();
    deselbytes = 1;
    int tries = 10;
    int i;
    for (i = 0; i < tries; i++)
    {
        select();
        if (sendCmd(0, 0) == 1) break;
        if (i) deselbytes = 255;
        deselect();
        deselbytes = 1;
        udelay(i * 10);
    }
    if (i >= tries) return RESULT_COMM_ERROR;
    if (sendCmd(8, 0x1aa) == 1)
    {
        uint32_t ocr = readR3();
        if (ocr == 0x1aa)
        {
            long timeout = TIMEOUT_SETUP(1000000);
            int result = 1;
            while (!TIMEOUT_EXPIRED(timeout) && result == 1) result = sendCmd(0x80 | 41, 1 << 30);
            if (result == 0 && sendCmd(58, 0) == 0)
            {
                uint32_t ocr = readR3();
                if (ocr & (1 << 30)) sdhc = true;
                initialized = true;
            }
        }
    }
    if (!initialized)
    {
        long timeout = TIMEOUT_SETUP(1000000);
        int result = 1;
        uint8_t cmd = 0x80 | 41;
        while (!TIMEOUT_EXPIRED(timeout))
        {
            result = sendCmd(cmd, 0);
            if (result == 0) break;
            if (result == 1) continue;
            if (cmd == 1) break;
            cmd = 1;
        }
        if (result == 0 && sendCmd(16, 512) == 0) initialized = true;
    }
    deselect();
    if (!initialized) return RESULT_COMM_ERROR;
    frequency = maxSpeed;
    select();
    uint8_t csd[16];
    if (sendCmd(9, 0) == 0 && readBlock(csd, 16) && (csd[0] >> 7) == 0)
    {
        if ((csd[0] >> 6) == 1) pageCount = (((csd[7] & 0x3f) << 16) + (csd[8] << 8) + csd[9] + 1) << 10;
        else pageCount = (((csd[6] & 3) << 10) + (csd[7] << 2) + (csd[8] >> 6) + 1)
                      << (((csd[9] & 3) << 1) + (csd[10] >> 7) + (csd[5] & 0xf) - 7);
    }
    else
    {
        initialized = false;
        deselect();
        return RESULT_COMM_ERROR;
    }
    deselect();
    return RESULT_OK;
}

enum Storage::Result SDHC_SPI::getStatus()
{
    return initialized ? RESULT_OK : RESULT_INVALID_STATE;
}

enum Storage::Result SDHC_SPI::readInternal(uint32_t page, uint32_t len, void* buf)
{
    enum Storage::Result rc = RESULT_COMM_ERROR;
    select();
    if (len == 1)
    {
        if (sendCmd(17, page) == 0 && readBlock(buf, 512))
            rc = RESULT_OK;
    }
    else if (sendCmd(18, page) == 0)
    {
        while (len--)
        {
            if (!readBlock(buf, 512))
            {
                deselect();
                return RESULT_COMM_ERROR;
            }
            buf = (void*)(((uintptr_t)buf) + 512);
        }
        if (sendCmd(12, 0) == 0) rc = RESULT_OK;
    }
    deselect();
    return rc;
}

enum Storage::Result SDHC_SPI::read(uint32_t page, uint32_t len, void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    if (!sdhc) page *= 512;
    enum Storage::Result rc = readInternal(page, len, buf);
    if (rc == RESULT_OK) return rc;
    reset();
    return readInternal(page, len, buf);
}

enum Storage::Result SDHC_SPI::writeInternal(uint32_t page, uint32_t len, const void* buf)
{
    enum Storage::Result rc = RESULT_COMM_ERROR;
    select();
    if (len == 1)
    {
        if (sendCmd(24, page) == 0 && writeBlock(buf, 512, 0xfe) && waitIdle()) rc = RESULT_OK;
    }
    else if (sendCmd(0x80 | 23, len) == 0 && sendCmd(25, page) == 0)
    {
        while (len--)
        {
            if (!writeBlock(buf, 512, 0xfc))
            {
                deselect();
                return RESULT_COMM_ERROR;
            }
            buf = (void*)(((uintptr_t)buf) + 512);
        }
        if (writeBlock(NULL, 0, 0xfd) && waitIdle()) rc = RESULT_OK;
    }
    deselect();
    return rc;
}

enum Storage::Result SDHC_SPI::write(uint32_t page, uint32_t len, const void* buf)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    if (!sdhc) page *= 512;
    enum Storage::Result rc = writeInternal(page, len, buf);
    if (rc == RESULT_OK) return rc;
    reset();
    return writeInternal(page, len, buf);
}

enum Storage::Result SDHC_SPI::eraseInternal(uint32_t page, uint32_t len)
{
    enum Storage::Result rc = RESULT_COMM_ERROR;
    select();
    if (sendCmd(32, page) == 0 && sendCmd(33, page + len - 1) == 0 && sendCmd(38, 0) == 0 && waitIdle()) rc = RESULT_OK;
    deselect();
    return rc;
}

enum Storage::Result SDHC_SPI::erase(uint32_t page, uint32_t len)
{
    if (!initialized) return RESULT_INVALID_STATE;
    if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
    if (!len) return RESULT_OK;
    if (!sdhc) page *= 512;
    enum Storage::Result rc = eraseInternal(page, len);
    if (rc == RESULT_OK) return rc;
    reset();
    return eraseInternal(page, len);
}
