#include "global.h"
#include "device/nrf/nrf_spi.h"
#include "sys/time.h"
#include "sys/util.h"


NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::access(uint32_t command, void* data, int len)
{
    select();
    NRF::SPI::Status status(pushByte(command));
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
    return access(Cmd_ReadReg | reg, data, len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writeReg(int reg, const void* data, int len)
{
    return access(Cmd_WriteReg | reg, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::readPacket(void* data, int len)
{
    return access(Cmd_ReadPacket, data, len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writePacket(const void* data, int len)
{
    return access(Cmd_WritePacket, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::flushTx()
{
    return access(Cmd_FlushTx, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::flushRx()
{
    return access(Cmd_FlushRx, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::reuseTxPacket()
{
    return access(Cmd_ReuseTx, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::getRxPacketSize(uint8_t* size)
{
    return access(Cmd_GetRxSize, size, 1);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writeAckPayload(int pipe, const void* data, int len)
{
    return access(Cmd_WriteACKPayload | pipe, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::writePacketNoAck(const void* data, int len)
{
    return access(Cmd_WriteBroadcastPacket, const_cast<void*>(data), len);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::getStatus()
{
    return access(Cmd_GetStatus, NULL, 0);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::SPI::activate(uint8_t feature)
{
    return access(Cmd_Activate, &feature, 1);
}

bool NRF_OPTIMIZE NRF::SPI::stayAwake(bool on)
{
    return SPI::Device::stayAwake(on);
}
