#pragma once
#include "global.h"
#include "sensor.h"
#include "lib/configstore/configstore.h"


#ifndef SENSORNODE_STORAGE_OPTIMIZE
#define SENSORNODE_STORAGE_OPTIMIZE
#endif


#define CONFIG_VERSION 1
union __attribute__((packed,aligned(4))) Config
{
    uint8_t u8[24];
    uint32_t u32[6];
    struct __attribute__((packed))
    {
        uint8_t version;
        uint8_t localId;
        uint8_t maxCommTimeout;
        uint8_t idleBeaconInterval;
        uint32_t serialNumber;
        BoardConfig boardConfig;
    };
};

union __attribute__((packed,aligned(4))) MetaEntry
{
    uint8_t u8[16];
    uint32_t u32[4];
    struct __attribute__((packed))
    {
        uint32_t measurementId;
        uint32_t boardTime;
        uint32_t unixTime;
        uint16_t clientDevice;
        uint8_t numSensors;
        uint8_t flags : 7;
        bool invalid : 1;
    };
};

class SensorNodeStorageDriver
{
public:
    Storage* configStorage;
    Storage* metaStorage;
    Storage* dataStorage;

    SensorNodeStorageDriver(Storage* configStorage, Storage* metaStorage, Storage* dataStorage)
        : configStorage(configStorage), metaStorage(metaStorage), dataStorage(dataStorage) {}
    virtual void init() = 0;
    virtual void stayAwake(bool on) = 0;
};


extern SensorNodeStorageDriver* storageDriver;
extern ConfigData<Config, 32> config;
extern ConfigStore configStore;
extern uint32_t measurementId;
extern uint32_t measurementTime;


extern void eraseAllData();
extern void makeSpaceForSensorMeta();
extern void writeDataPoint(uint8_t sensor, uint32_t data);
extern void writeMeta(uint32_t unixTime, uint16_t clientDevice, uint8_t flags);
extern bool getHistorySensorAttributes(uint32_t measurementId, uint8_t sensorId, SensorAttributes* ptr);
extern bool getHistoryMeta(uint32_t* fromMeasId, uint32_t* fromTime, MetaEntry* ptr);
extern int getHistorySensorData(uint32_t measurementId, uint32_t* fromTime, uint8_t sensorId, void* ptr, uint32_t maxLen);
extern void initStorage();
extern void saveConfig();
