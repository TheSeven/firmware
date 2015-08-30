#include "global.h"
#include "app/sensornode/sensor/humidity/htu21d.h"


uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorHumidityHTU21D::readValue()
{
    return sensor->readRawHumidity();
}
