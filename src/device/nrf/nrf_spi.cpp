#include "global.h"
#include "device/nrf/nrf_spi.h"
#include "sys/time.h"
#include "sys/util.h"


NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::access(uint32_t command, void* data, int len)
{
    select();
    NRF::SPI::Status status = { pushByte(command) };
    if (len)
    {
        if (command & 0x100) pushBuffer(data, NULL, len);
        else pushBuffer(NULL, data, len);
    }
    deselect();
    return status;
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::readReg(int reg, void* data, int len)
{
    return access(reg, data, len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writeReg(int reg, const void* data, int len)
{
    return access(0x120 | reg, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::readPacket(void* data, int len)
{
    return access(0x61, data, len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writePacket(const void* data, int len)
{
    return access(0x1a0, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::flushTx()
{
    return access(0xe1, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::flushRx()
{
    return access(0xe2, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::reuseTxPacket()
{
    return access(0xe3, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::getRxPacketSize(uint8_t* size)
{
    return access(0x60, size, 1);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writeAckPayload(int pipe, const void* data, int len)
{
    return access(0x1a8 | pipe, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writePacketNoAck(const void* data, int len)
{
    return access(0x1b0, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::getStatus()
{
    return access(0xff, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::activate(uint8_t feature)
{
    return access(0x150, &feature, 1);
}
