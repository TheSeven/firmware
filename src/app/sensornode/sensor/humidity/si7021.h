#pragma once
#include "global.h"
#include "app/sensornode/sensor.h"
#include "device/si7021/si7021.h"


class SensorNodeSensorHumiditySI7021 : public SensorNodeSensorDriver
{
    SI7021* sensor;

public:
    SensorNodeSensorHumiditySI7021() {}
    SensorNodeSensorHumiditySI7021(SI7021* sensor, uint16_t interval, uint8_t offset)
        : SensorNodeSensorDriver(2, 0x48313248, 0, interval, offset), sensor(sensor), lastReadTime(-1) {}

    uint32_t readValue();
    int lastReadTime;
};
