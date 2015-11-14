#include "global.h"
#include "app/sensornode/sensor/humidity/si7021.h"
#include "app/sensornode/common.h"


uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorHumiditySI7021::readValue()
{
    lastReadTime = now;
    return sensor->readRawHumidity();
}
