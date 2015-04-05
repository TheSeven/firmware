#include "global.h"
#include "device/nrf/se8r01/se8r01.h"
#include "sys/time.h"
#include "sys/util.h"


enum EntryType
{
    End = 0,
    Delay = 0,
    Len_1 = 1,
    Len_2 = 2,
    Len_3 = 3,
    Len_4 = 4,
    Len_5 = 5,
    SwitchBank = 6,
};

const uint8_t NRF::SE8R01::initSequence[] =
{
    Reg_Config | (Len_1 << 5), 0x01,
    Reg_RfChannel | (Len_1 << 5), 0x32,
    Reg_RfSetup | (Len_1 << 5), 0x48,
    Reg_GuardCtl | (Len_1 << 5), 0x77,
    (SwitchBank << 5),
    IntReg_PllCtl0 | (Len_4 << 5), 0x40, 0x00, 0x10, 0xe6,
    IntReg_CalCtl | (Len_5 << 5), 0x20, 0x08, 0x50, 0x40, 0x50,
    IntReg_IfFreq | (Len_3 << 5), 0x00, 0x00, 0x1e,
    IntReg_FDev | (Len_1 << 5), 0x29,
    IntReg_DacCalHi | (Len_1 << 5), 0x7f,
    IntReg_AgcGain | (Len_4 << 5), 0x02, 0xc1, 0xeb, 0x1c,
    IntReg_RfIvGen | (Len_4 << 5), 0x97, 0x64, 0x00, 0x81,
    (SwitchBank << 5),
    Reg_Config | (Len_1 << 5), 0x03,
    (Delay << 5) | 15,
    Reg_Config | (Len_1 << 5), 0x01,
    (Delay << 5) | 25,
    Reg_Config | (Len_1 << 5), 0x03,
    (Delay << 5) | 15,
    Reg_Config | (Len_1 << 5), 0x01,
    (Delay << 5) | 25,
    Reg_Config | (Len_1 << 5), 0x03,
    Reg_Tuning | (Len_5 << 5), 0x28, 0x32, 0x80, 0x90, 0x00,
    (Delay << 5) | 1,
    (SwitchBank << 5),
    IntReg_PllCtl0 | (Len_4 << 5), 0x40, 0x01, 0x30, 0xe2,
    IntReg_CalCtl | (Len_5 << 5), 0x29, 0x89, 0x55, 0x40, 0x50,
    IntReg_RxCtrl | (Len_4 << 5), 0x55, 0xc2, 0x09, 0xac,
    IntReg_FAgcCtrl | (Len_4 << 5), 0x00, 0x14, 0x08, 0x29,
    IntReg_AgcGain | (Len_4 << 5), 0x02, 0xc1, 0xcb, 0x1c,
    IntReg_RfIvGen | (Len_4 << 5), 0x97, 0x64, 0x00, 0x01,
    IntReg_TestPkDet | (Len_4 << 5), 0x2a, 0x04, 0x00, 0x7d,
    (SwitchBank << 5),
    (End << 5),
};


void NRF::SE8R01::switchBank()
{
    activate(Feature_SwitchRegisterBank);
}

void NRF::SE8R01::init()
{
    if (getStatus().b.regBank) switchBank();
    const uint8_t* ptr = initSequence;
    while (uint8_t cmd = *ptr++)
    {
        if ((cmd >> 5) == Delay) udelay(2 * (cmd & 0x1f));
        else if ((cmd >> 5) == SwitchBank) switchBank();
        else
        {
            writeReg(cmd & 0x1f, ptr, cmd >> 5);
            ptr += cmd >> 5;
        }
    }
}

void NRF::SE8R01::configure(Configuration* config)
{
    NRF::SE8R01::Config configOff = { 0 };
    configOff.b.maskDataReceived = true;
    configOff.b.maskDataSent = true;
    configOff.b.maskMaxRetrans = true;
    writeReg(Reg_Config, &configOff, sizeof(configOff));
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
    writeReg(Reg_Tuning, &config->tuning, sizeof(config->tuning));
    writeReg(Reg_GuardCtl, &config->guardCtl, sizeof(config->guardCtl));
    writeReg(Reg_Config, &config->config, sizeof(config->config));
}

NRF::SPI::Status NRF::SE8R01::handleIRQ()
{
    Status status = getStatus();
    uint8_t length = 0;
    uint8_t data[32];
    TxObserve txObserve;
    if (status.b.dataReceived)
    {
        getRxPacketSize(&length);
        readPacket(data, length);
    }
    if (packetTransmitted && (status.b.maxRetrans || status.b.dataSent))
        readReg(Reg_TxObserve, &txObserve, sizeof(txObserve));
    if (status.b.maxRetrans || status.b.dataSent || status.b.dataReceived)
        status = writeReg(Reg_Status, &status, sizeof(status));
    if (packetReceived && status.b.dataReceived)
        packetReceived(status.b.rxPipe, data, length);
    if (packetTransmitted && (status.b.maxRetrans || status.b.dataSent))
        packetTransmitted(!status.b.maxRetrans, txObserve.b.retransCount);
    return status;
}
