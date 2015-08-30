#include "global.h"
#include "app/sensornode/sensor/temperature/ds1820.h"


uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorTemperatureDS1820::readValue()
{
    return ds1820.readRawTemperature();
}
