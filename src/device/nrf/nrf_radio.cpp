#include "global.h"
#include "device/nrf/nrf_radio.h"
#include "sys/time.h"
#include "sys/util.h"


NRF::SPI::Status NRF_OPTIMIZE NRF::Radio::setTxAddress(const uint8_t* address, int length)
{
    writeReg(Reg_RxPipeAddress0, address, length);
    return writeReg(Reg_TxAddress, address, length);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::Radio::setRxAddress(int pipe, const uint8_t* address, int length)
{
    return writeReg(Reg_RxPipeAddress0 + pipe, address, length);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::Radio::setMode(bool receive)
{
    Config config;
    readReg(Reg_Config, &config, sizeof(config));
    bool powerUp = config.b.powerUp;
    config.b.powerUp = false;
    config.b.role = (Config::b::Role)receive;
    writeReg(Reg_Config, &config, sizeof(config));
    config.b.powerUp = powerUp;
    return writeReg(Reg_Config, &config, sizeof(config));
}

NRF::SPI::Status NRF_OPTIMIZE NRF::Radio::power(bool on)
{
    Config config;
    readReg(Reg_Config, &config, sizeof(config));
    config.b.powerUp = on;
    return writeReg(Reg_Config, &config, sizeof(config));
}

NRF::SPI::Status NRF_OPTIMIZE NRF::Radio::setPacketSize(int pipe, int length)
{
    return writeReg(Reg_RxDataLength0 + pipe, &length, 1);
}

NRF::SPI::Status NRF_OPTIMIZE NRF::Radio::transmit(int pipe, const void* data, int length)
{
    Status status = getStatus();
    if (status.b.txFull) return status;
    if (pipe < -1) return writePacketNoAck(data, length);
    if (pipe >= 0) return writeAckPayload(pipe, data, length);
    return writePacket(data, length);
}
