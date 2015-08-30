#pragma once
#include "global.h"
#include "common.h"
#include "sensor.h"
#include "storage.h"
#include "rtc.h"
#include "lib/nrfbeacon/manager.h"


#ifndef SENSORNODE_RADIO_OPTIMIZE
#define SENSORNODE_RADIO_OPTIMIZE
#endif


class SensorNodeRadioDriver
{
public:
    NRF::Radio* radio;
    NRFBeacon::Manager* beacon;

    SensorNodeRadioDriver(NRF::Radio* radio, NRFBeacon::Manager* beacon) : radio(radio), beacon(beacon) {}

    virtual void sleep() = 0;
    virtual void wake() = 0;
    virtual bool detectIRQ() = 0;
};


struct __attribute__((packed,aligned(4))) RadioPacket
{
    enum RadioPacketType
    {
        ConfigGet = 0x10,
        ConfigSet = 0x14,
        ConfigData = 0x18,
        ConfigResult = 0x1c,
        Command = 0x20,
        CommandResult = 0x28,
        DownloadMeta = 0xe0,
        DownloadHistory = 0xe1,
        DownloadPacket = 0xef,
    };

    enum ConfigType
    {
        HistorySensorConfig = 0x00,
        LiveSensorConfig = 0x01,
        BoardConfig = 0xff,
    };

    enum BoardConfigParameter
    {
        CurrentMeasTime = 0x00,
        CurrentRtcState = 0x01,
        BeaconDefaultConfig = 0xff,
    };

    enum Command
    {
        RebootCommand = 0x0000,
        SaveConfigCommand = 0x0100,
        TimestampCommand = 0x0200,
        FactoryProgramming = 0x00ff,
    };

    enum StatusCode
    {
        StatusSuccess = 0x00000000,
        StatusUnknownCommand = 0x00000001,
        StatusInvalidState = 0x00000002,
        StatusInvalidArgument = 0x00000003,
        StatusOutOfMemory = 0x00000004,
        StatusHardwareError = 0x00000005,
        StatusDataGap = 0x00000006,
    };

    union __attribute__((packed)) ConfigArgument
    {
        uint8_t u8[28];
        struct __attribute__((packed))
        {
            uint32_t measurementId;
        } historySensorConfig;
    };

    union __attribute__((packed)) ConfigPayload
    {
        uint8_t u8[28];
        struct __attribute__((packed))
        {
            uint32_t measurementId;
            SensorAttributes attr;
        } historySensorConfig;
        SensorMeta liveSensorConfig;
        struct __attribute__((packed))
        {
            uint32_t currentMeasId;
            uint32_t currentMeasTime;
        } currentMeasTime;
        RTCState currentRtcState;
        struct __attribute__((packed))
        {
            uint8_t maxTimeout;
            uint8_t idleInterval;
        } beaconDefaultConfig;
    };

    union __attribute__((packed)) CommandArgument
    {
        uint8_t u8[28];
        struct __attribute__((packed))
        {
            uint32_t unixTime;
            uint16_t clientId;
        } timestamp;
        struct __attribute__((packed))
        {
            uint32_t serialNumber;
            uint8_t platformArgs[24];
        } factoryProgramming;
    };

    enum RadioPacketType type : 8;
    uint8_t req;
    union __attribute__((packed))
    {
        struct __attribute__((packed))
        {
            uint16_t seq;
            union __attribute__((packed))
            {
                uint8_t u8[28];
                MetaEntry meta;
            } payload;
        } downloadPacket;
        struct __attribute__((packed))
        {
            uint16_t size;
            uint32_t fromMeasurementId;
            uint32_t fromTime;
        } downloadMeta;
        struct __attribute__((packed))
        {
            uint16_t size;
            uint32_t measurementId;
            uint32_t dataOffset;
            uint8_t sensorId;
        } downloadMeasurements;
        struct __attribute__((packed))
        {
            enum ConfigType type : 8;
            uint8_t id;
            ConfigArgument arg;
        } configGet;
        struct __attribute__((packed))
        {
            enum ConfigType type : 8;
            uint8_t id;
            ConfigPayload payload;
        } configSet;
        struct __attribute__((packed))
        {
            enum ConfigType type : 8;
            uint8_t id;
            ConfigPayload payload;
        } config;
        struct __attribute__((packed))
        {
            enum ConfigType type : 8;
            uint8_t id;
            enum StatusCode status : 32;
        } configResult;
        struct __attribute__((packed))
        {
            enum Command cmd : 16;
            CommandArgument arg;
        } command;
        struct __attribute__((packed))
        {
            enum Command cmd : 16;
            enum StatusCode status : 32;
            uint32_t arg0;
        } commandResult;
    } args;
};


extern NRFBeacon::Identification deviceId;
extern SensorNodeRadioDriver* radioDriver;

extern int nextBeacon;
extern int commTimeout;

extern void transmittedHandler(bool success, int retransmissions);
extern void receivedHandler(int pipe, uint8_t* data, int length);
extern void applyBeaconSettings(uint8_t idleInterval, uint8_t maxTimeout, uint8_t localId);
extern void handleRadio();

extern RadioPacket::StatusCode sensornode_handle_custom_command(RadioPacket* packet, int length);
extern void sensornode_factory_programming(uint8_t* args, int length);
