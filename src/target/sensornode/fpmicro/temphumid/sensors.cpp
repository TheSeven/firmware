#include "global.h"
#include "app/sensornode/sensor.h"
#include "app/sensornode/common.h"
#include "app/sensornode/storage.h"
#include "app/sensornode/sensor/powersupply/stm32vcc.h"
#include "app/sensornode/sensor/temperature/si7021.h"
#include "app/sensornode/sensor/humidity/si7021.h"
#include "soc/stm32/f0/i2c.h"


SensorNodeSensorDriver* sensors[SENSORNODE_MAXSENSORS];
SensorNodeSensorPowerSupplySTM32VCC vccSensor;
SensorNodeSensorHumiditySI7021 humSensor;
SensorNodeSensorTemperatureSI7021 tempSensor;
SI7021 si7021(&STM32::I2C::I2C1);


void SENSORNODE_TARGET_OPTIMIZE sensornode_init_sensors()
{
    sensorCount = 0;

    new(&vccSensor) SensorNodeSensorPowerSupplySTM32VCC(config.data.boardConfig.sensorInterval[0],
                                                        config.data.boardConfig.sensorOffset[0]);
    memcpy(vccSensor.meta.attr.sensorName, "Battery voltage", 15);
    sensors[sensorCount++] = &vccSensor;
    new(&humSensor) SensorNodeSensorHumiditySI7021(&si7021, config.data.boardConfig.sensorInterval[1],
                                                   config.data.boardConfig.sensorOffset[1]);
    memcpy(humSensor.meta.attr.sensorName, "Humidity", 8);
    sensors[sensorCount++] = &humSensor;
    new(&tempSensor) SensorNodeSensorTemperatureSI7021(&si7021, &humSensor, config.data.boardConfig.sensorInterval[2],
                                                       config.data.boardConfig.sensorOffset[2]);
    memcpy(tempSensor.meta.attr.sensorName, "Temperature", 11);
    sensors[sensorCount++] = &tempSensor;
}

uint16_t SENSORNODE_TARGET_OPTIMIZE sensornode_get_persistent_interval(uint8_t sensorId)
{
    return config.data.boardConfig.sensorInterval[sensorId];
}

void SENSORNODE_TARGET_OPTIMIZE sensornode_set_persistent_sensor_config(uint8_t sensorId, SensorMeta* meta)
{
    config.data.boardConfig.sensorInterval[sensorId] = meta->attr.interval;
    config.data.boardConfig.sensorOffset[sensorId] = meta->offset;
}

bool SENSORNODE_TARGET_OPTIMIZE sensornode_flash_write_safe()
{
    //uint32_t voltage = psuSensor.readValue();
    //if ((voltage & 0xffff) < 1000) return false;
    //if ((voltage >> 16) < 1000) return false;
    return true;
}

void sensornode_enable_sensors()
{
}

void sensornode_disable_sensors()
{
}
