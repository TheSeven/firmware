#include "global.h"
#include "app/sensornode/sensor/temperature/ds1820.h"


SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorTemperatureDS1820::SensorNodeSensorTemperatureDS1820(OneWire::Bus* bus,
                                                                                                uint64_t deviceId,
                                                                                                uint16_t interval,
                                                                                                uint8_t offset)
    : SensorNodeSensorDriver(0x20185344, 2, 0, interval, offset), ds1820(bus, deviceId)
{
}

uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorTemperatureDS1820::readValue()
{
    return ds1820.readRawTemperature();
}
