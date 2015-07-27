#include "global.h"
#include "device/nrf/nrf24l01p/nrf24l01p.h"
#include "sys/time.h"
#include "sys/util.h"


void NRF_OPTIMIZE NRF::NRF24L01P::configure(Configuration* config)
{
    NRF::NRF24L01P::Config configOff = { 0 };
    configOff.b.maskDataReceived = true;
    configOff.b.maskDataSent = true;
    configOff.b.maskMaxRetrans = true;
    stayAwake(true);
    writeReg(Reg_Config, &configOff, sizeof(configOff));
    configOff.d8 = config->config.d8;
    configOff.b.powerUp = false;
    flushTx();
    Status status = flushRx();
    writeReg(Reg_Status, &status, sizeof(status));
    writeReg(Reg_FeatureCtl, &config->featureCtl, sizeof(config->featureCtl));
    writeReg(Reg_AutoAckCtl, &config->autoAckCtl, sizeof(config->autoAckCtl));
    writeReg(Reg_RxPipeEnable, &config->rxPipeEnable, sizeof(config->rxPipeEnable));
    writeReg(Reg_AddressCtl, &config->addressCtl, sizeof(config->addressCtl));
    writeReg(Reg_RetransCtl, &config->retransCtl, sizeof(config->retransCtl));
    writeReg(Reg_RfChannel, &config->rfChannel, sizeof(config->rfChannel));
    writeReg(Reg_RfSetup, &config->rfSetup, sizeof(config->rfSetup));
    writeReg(Reg_DynLengthCtl, &config->dynLengthCtl, sizeof(config->dynLengthCtl));
    writeReg(Reg_Config, &configOff, sizeof(configOff));
    writeReg(Reg_Config, &config->config, sizeof(config->config));
    stayAwake(false);
}


NRF::SPI::Status NRF_OPTIMIZE NRF::NRF24L01P::handleIRQ()
{
    stayAwake(true);
    Status status = getStatus();
    Status result = status;
    uint8_t length = 0;
    uint8_t data[32];
    TxObserve txObserve;
    if (packetTransmitted && (status.b.maxRetrans || status.b.dataSent))
        result = readReg(Reg_TxObserve, &txObserve, sizeof(txObserve));
    if (status.b.maxRetrans || status.b.dataSent || status.b.dataReceived)
        result = writeReg(Reg_Status, &status, sizeof(status));
    if (status.b.dataReceived)
        while (true)
        {
            stayAwake(true);
            FifoStatus fifoStatus;
            result = readReg(Reg_FifoStatus, &fifoStatus, sizeof(fifoStatus));
            if (fifoStatus.b.rxEmpty) break;
            getRxPacketSize(&length);
            readPacket(data, length);
            stayAwake(false);
            if (packetReceived) packetReceived(status.b.rxPipe, data, length);
        }
    stayAwake(false);
    if (packetTransmitted && (status.b.maxRetrans || status.b.dataSent))
        packetTransmitted(!status.b.maxRetrans, txObserve.b.retransCount);
    return result;
}
