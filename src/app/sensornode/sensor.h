#pragma once
#include "global.h"


#ifndef SENSORNODE_SENSOR_OPTIMIZE
#define SENSORNODE_SENSOR_OPTIMIZE
#endif


struct __attribute__((packed,aligned(4))) SensorAttributes
{
    uint32_t dataFormat;
    uint8_t bytesPerPoint;
    uint8_t sensorType;
    uint16_t interval;
    char sensorName[16];
};

struct __attribute__((packed,aligned(4))) SensorMeta
{
    uint8_t offset;
    uint32_t : 24;
    SensorAttributes attr;
};

class SensorNodeSensorDriver
{
public:
    SensorMeta meta;
    uint32_t writePtr;
    int nextTime;

    constexpr SensorNodeSensorDriver() : meta{0, {0, 0, 0, 0, {0}}}, writePtr(0), nextTime(0) {}
    constexpr SensorNodeSensorDriver(uint32_t dataFormat, uint8_t bytesPerPoint, uint8_t sensorType,
                                     uint16_t interval, uint8_t offset)
        : meta{offset, {dataFormat, bytesPerPoint, sensorType, interval, {0}}}, writePtr(0), nextTime(0) {}
    virtual uint32_t readValue() = 0;
};


extern SensorNodeSensorDriver* sensors[SENSORNODE_MAXSENSORS];


extern void sensornode_init_sensors();
extern uint16_t sensornode_get_persistent_interval(uint8_t sensorId);
extern void sensornode_set_persistent_sensor_config(uint8_t sensorId, SensorMeta* meta);
