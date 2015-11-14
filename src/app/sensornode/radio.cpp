#include "global.h"
#include "radio.h"
#include "sys/time.h"
#include "sys/util.h"



struct DownloadJob
{
    uint32_t measurementId;
    uint32_t offset;
    uint16_t sizeLeft;
    uint16_t seq;
    uint8_t req;
    enum Type
    {
        None = 0,
        Meta = 1,
        SensorData = 2,
    } type : 8;
    uint8_t sensorId;
} currentDownload;


NRFBeacon::Identification deviceId{{SENSORNODE_HW_VID, SENSORNODE_HW_PID, 0xffffffff},
                                   {SENSORNODE_PROT_VID, SENSORNODE_PROT_TYPE, SENSORNODE_PROT_VER},
                                   {SENSORNODE_FW_VID, SENSORNODE_FW_TYPE, SENSORNODE_FW_VER}};

int nextBeacon;
int commTimeout = -1;
uint16_t beaconInterval;
bool beaconRTC;
bool rebootPending = false;
uint8_t txPending = 0;


void SENSORNODE_RADIO_OPTIMIZE applyBeaconSettings(uint8_t idleInterval, uint8_t maxTimeout, uint8_t localId)
{
    deviceId.hw.serial = sensornode_get_hw_serial();
    radioDriver->beacon->idleInterval = idleInterval;
    radioDriver->beacon->maxTimeout = maxTimeout;
    radioDriver->beacon->localId = localId;
    radioDriver->beacon->payload = &liveData;
    radioDriver->beacon->payloadLen = liveDataSize;
}


void SENSORNODE_RADIO_OPTIMIZE updateBeaconInterval()
{
    if (radioDriver->beacon->interval == beaconInterval) return;
    beaconInterval = radioDriver->beacon->interval;
    beaconRTC = beaconInterval > 100;
}


void SENSORNODE_RADIO_OPTIMIZE calcNextBeacon()
{
    if (beaconRTC) nextBeacon = now + beaconInterval / 100;
    else nextBeacon = TIMEOUT_SETUP((beaconInterval - 1) * 10000);
}


void SENSORNODE_RADIO_OPTIMIZE calcTimeout()
{
    commTimeout = now + (radioDriver->beacon->timeout ? radioDriver->beacon->timeout : 3600);
}


static void SENSORNODE_RADIO_OPTIMIZE flushTx()
{
    radioDriver->radio->flushTx();
    txPending = 0;
}


static void SENSORNODE_RADIO_OPTIMIZE handleTxStatus(NRF::SPI::Status status)
{
    if (txPending > 1) txPending = 1;
    if (status.b.txFull) flushTx();
    else txPending++;
}


static void SENSORNODE_RADIO_OPTIMIZE transmit(void* data, int length)
{
    handleTxStatus(radioDriver->radio->transmit(-1, data, length));
}


void SENSORNODE_RADIO_OPTIMIZE handleDownload()
{
    if (currentDownload.type == DownloadJob::None) return;
    RadioPacket packet;
    while (true)
    {
        NRF::SPI::Status status = radioDriver->radio->getStatus();
        if (status.b.txFull || currentDownload.type == DownloadJob::None) break;
        uint32_t maxLength = MIN(currentDownload.sizeLeft, sizeof(packet.args.downloadPacket.payload));
        uint32_t length = 0;
        if (maxLength)
        {
            switch (currentDownload.type)
            {
            case DownloadJob::Meta:
                if (getHistoryMeta(&currentDownload.measurementId, &currentDownload.offset,
                                   &packet.args.downloadPacket.payload.meta))
                    length = sizeof(packet.args.downloadPacket.payload.meta);
                break;
            case DownloadJob::SensorData:
                length = getHistorySensorData(currentDownload.measurementId, &currentDownload.offset,
                                              currentDownload.sensorId, &packet.args.downloadPacket.payload, maxLength);
                if (length < 0)
                {
                    currentDownload.type = DownloadJob::None;
                    packet.type = RadioPacket::CommandResult;
                    packet.req = currentDownload.req;
                    packet.args.commandResult.cmd = (enum RadioPacket::Command)0;
                    packet.args.commandResult.status = RadioPacket::StatusDataGap;
                    packet.args.commandResult.arg0 = -length;
                    transmit(&packet, sizeof(packet) - sizeof(packet.args) + sizeof(packet.args.commandResult));
                    return;
                }
                break;
            case DownloadJob::None: break;
            }
            if (length > maxLength) length = maxLength;
            currentDownload.sizeLeft -= length;
        }
        packet.type = RadioPacket::DownloadPacket;
        packet.req = currentDownload.req;
        packet.args.downloadPacket.seq = currentDownload.seq++;
        if (!length) currentDownload.type = DownloadJob::None;
        transmit(&packet, sizeof(packet) - sizeof(packet.args.downloadPacket.payload) + length);
    }
}


void SENSORNODE_RADIO_OPTIMIZE handleRadio()
{
    bool sendBeacon = false;

    if (radioDriver->detectIRQ()) radioDriver->radio->handleIRQ();

    if (TIME_AFTER(now, commTimeout))
    {
        radioDriver->beacon->timeoutExpired();
        calcTimeout();
        updateBeaconInterval();
        calcNextBeacon();
        sendBeacon = true;
    }

    if (beaconRTC) sendBeacon |= TIME_AFTER(now, nextBeacon);
    else sendBeacon |= TIMEOUT_EXPIRED(nextBeacon);
    if (sendBeacon)
    {
        if (radioDriver->sleeping) radioDriver->wake();
        handleTxStatus(radioDriver->beacon->sendBeacon());
        calcNextBeacon();
    }

    if (!beaconRTC) deepSleep = false;
}


void SENSORNODE_RADIO_OPTIMIZE transmittedHandler(bool success, int retransmissions)
{
    if (rebootPending) sensornode_reboot();
    else if (!success)
    {
        currentDownload.type = DownloadJob::None;
        flushTx();
    }
    else
    {
        handleDownload();
        calcTimeout();
        if (txPending) txPending--;
    }
    if (txPending > 1) txPending = 1;
    if (!txPending) radioDriver->sleep();
}


void SENSORNODE_RADIO_OPTIMIZE receivedHandler(int pipe, uint8_t* data, int length)
{
    if (rebootPending) return;
    currentDownload.type = DownloadJob::None;
    if (length >= 2 && data[0] == 0xff && data[1] == 0xff)
    {
        radioDriver->beacon->processPacket(data, length);
        updateBeaconInterval();
    }
    else
    {
        if (length < 4) return;
        RadioPacket* packet = (RadioPacket*)data;
        switch (packet->type)
        {
        case RadioPacket::ConfigGet:
            packet->type = RadioPacket::ConfigData;
            switch (packet->args.configGet.type)
            {
            case RadioPacket::HistorySensorConfig:
                if (length != 8 || !getHistorySensorAttributes(packet->args.configGet.arg.historySensorConfig.measurementId,
                                                               packet->args.configGet.id,
                                                               &packet->args.config.payload.historySensorConfig.attr))
                    length = -RadioPacket::StatusInvalidArgument;
                else length = sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.config)
                            - sizeof(packet->args.config.payload) + sizeof(packet->args.config.payload.historySensorConfig);
                break;
            case RadioPacket::LiveSensorConfig:
                if (length != 4 || packet->args.configGet.id >= sensorCount)
                {
                    length = -RadioPacket::StatusInvalidArgument;
                    break;
                }
                length = sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.config)
                       - sizeof(packet->args.config.payload) + sizeof(packet->args.config.payload.liveSensorConfig);
                memcpy(&packet->args.config.payload.liveSensorConfig, &sensors[packet->args.configGet.id]->meta,
                       sizeof(packet->args.config.payload.liveSensorConfig));
                packet->args.config.payload.liveSensorConfig.attr.interval = sensornode_get_persistent_interval(packet->args.configGet.id);
                break;
            case RadioPacket::BoardConfig:
                switch (packet->args.configGet.id)
                {
                case RadioPacket::CurrentMeasTime:
                    if (length != 4)
                    {
                        length = -RadioPacket::StatusInvalidArgument;
                        break;
                    }
                    length = sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.config)
                           - sizeof(packet->args.config.payload) + sizeof(packet->args.config.payload.currentMeasTime);
                    packet->args.config.payload.currentMeasTime.currentMeasId = measurementId;
                    packet->args.config.payload.currentMeasTime.currentMeasTime = measurementTime;
                    break;
                case RadioPacket::CurrentRtcState:
                    if (length != 4)
                    {
                        length = -RadioPacket::StatusInvalidArgument;
                        break;
                    }
                    length = sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.config)
                           - sizeof(packet->args.config.payload) + sizeof(packet->args.config.payload.currentRtcState);
                    rtcDriver->getState(&packet->args.config.payload.currentRtcState);
                    break;
                case RadioPacket::BeaconDefaultConfig:
                    if (length != 4)
                    {
                        length = -RadioPacket::StatusInvalidArgument;
                        break;
                    }
                    length = sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.config)
                           - sizeof(packet->args.config.payload) + sizeof(packet->args.config.payload.beaconDefaultConfig);
                    packet->args.config.payload.beaconDefaultConfig.maxTimeout = radioDriver->beacon->maxTimeout;
                    packet->args.config.payload.beaconDefaultConfig.idleInterval = radioDriver->beacon->idleInterval;
                    break;
                default:
                    length = -RadioPacket::StatusUnknownCommand;
                }
                break;
            default:
                length = -RadioPacket::StatusUnknownCommand;
            }
            if (length < 0)
            {
                packet->type = RadioPacket::ConfigResult;
                packet->args.configResult.status = (RadioPacket::StatusCode)-length;
                length = sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.configResult);
            }
            transmit(data, length);
            break;
        case RadioPacket::ConfigSet:
            packet->type = RadioPacket::ConfigResult;
            switch (packet->args.configSet.type)
            {
            case RadioPacket::LiveSensorConfig:
                if (length < 16 || packet->args.configSet.id >= sensorCount)
                {
                    packet->args.configResult.status = RadioPacket::StatusInvalidArgument;
                    break;
                }
                if (sensors[packet->args.configSet.id]->meta.offset != packet->args.config.payload.liveSensorConfig.offset)
                {
                    sensors[packet->args.configSet.id]->meta.offset = packet->args.config.payload.liveSensorConfig.offset;
                    updateLiveDataSize();
                }
#ifdef SENSORNODE_SUPPORT_SENSOR_NAME_CHANGE
                if (length > 16)
                {
                    memcpy(sensors[packet->args.configGet.id]->meta.attr.sensorName,
                           packet->args.config.payload.liveSensorConfig.attr.sensorName, length - 16);
                    memset(sensors[packet->args.configGet.id]->meta.attr.sensorName + length - 16, 0, 32 - length);
                }
                else memcpy(packet->args.config.payload.liveSensorConfig.attr.sensorName,
                            sensors[packet->args.configGet.id]->meta.attr.sensorName, 16);
#endif
                sensornode_set_persistent_sensor_config(packet->args.configSet.id, &packet->args.config.payload.liveSensorConfig);
                packet->args.configResult.status = RadioPacket::StatusSuccess;
                break;
            case RadioPacket::BoardConfig:
                switch (packet->args.configSet.id)
                {
                case RadioPacket::BeaconDefaultConfig:
                    if (length != 6)
                    {
                        packet->args.configResult.status = RadioPacket::StatusInvalidArgument;
                        break;
                    }
                    radioDriver->beacon->idleInterval = packet->args.config.payload.beaconDefaultConfig.idleInterval;
                    radioDriver->beacon->maxTimeout = packet->args.config.payload.beaconDefaultConfig.maxTimeout;
                    packet->args.configResult.status = RadioPacket::StatusSuccess;
                    break;
                default:
                    packet->args.configResult.status = RadioPacket::StatusUnknownCommand;
                }
                break;
            default:
                packet->args.configResult.status = RadioPacket::StatusUnknownCommand;
            }
            transmit(data, sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.configResult));
            break;
        case RadioPacket::Command:
            packet->type = RadioPacket::CommandResult;
            switch (packet->args.command.cmd)
            {
            case RadioPacket::RebootCommand:
                rebootPending = true;
                packet->args.commandResult.status = RadioPacket::StatusSuccess;
                break;
            case RadioPacket::SaveConfigCommand:
                saveConfig();
                packet->args.commandResult.status = RadioPacket::StatusSuccess;
                break;
            case RadioPacket::TimestampCommand:
                if (length < 10) packet->args.commandResult.status = RadioPacket::StatusInvalidArgument;
                else
                {
                    writeMeta(packet->args.command.arg.timestamp.unixTime, packet->args.command.arg.timestamp.clientId, 0);
                    packet->args.commandResult.status = RadioPacket::StatusSuccess;
                }
                break;
            case RadioPacket::BlinkCommand:
                blinkUntil = now + packet->args.command.arg.blink.time;
                packet->args.commandResult.status = RadioPacket::StatusSuccess;
                break;
            case RadioPacket::FactoryProgramming:
                if (length < 8) packet->args.commandResult.status = RadioPacket::StatusInvalidArgument;
                else
                {
                    config.data.serialNumber = packet->args.command.arg.factoryProgramming.serialNumber;
                    sensornode_factory_programming(packet->args.command.arg.factoryProgramming.platformArgs,
                                                   length - sizeof(*packet)
                                                 + sizeof(packet->args.command.arg.factoryProgramming.platformArgs));
                    saveConfig();
                    eraseAllData();
                    sensornode_reboot();
                }
                break;
            default:
                packet->args.commandResult.status = sensornode_handle_custom_command(packet, length);
            }
            transmit(data, sizeof(*packet) - sizeof(packet->args) + sizeof(packet->args.commandResult));
            break;
        case RadioPacket::DownloadMeta:
            currentDownload.req = packet->req;
            currentDownload.seq = 0;
            currentDownload.sizeLeft = packet->args.downloadMeta.size;
            currentDownload.measurementId = packet->args.downloadMeta.fromMeasurementId;
            currentDownload.offset = packet->args.downloadMeta.fromTime;
            currentDownload.type = DownloadJob::Meta;
            handleDownload();
            break;
        case RadioPacket::DownloadHistory:
            currentDownload.req = packet->req;
            currentDownload.seq = 0;
            currentDownload.sizeLeft = packet->args.downloadMeasurements.size;
            currentDownload.measurementId = packet->args.downloadMeasurements.measurementId;
            currentDownload.offset = packet->args.downloadMeasurements.dataOffset;
            currentDownload.sensorId = packet->args.downloadMeasurements.sensorId;
            currentDownload.type = DownloadJob::SensorData;
            handleDownload();
            break;
        default: break;
        }
    }
    calcTimeout();
    calcNextBeacon();
}
