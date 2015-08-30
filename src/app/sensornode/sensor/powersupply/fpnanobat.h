#pragma once
#include "global.h"
#include "app/sensornode/sensor.h"
#include "stm32vcc.h"


class SensorNodeSensorPowerSupplyFPNanoBat : public SensorNodeSensorDriver
{
public:
    SensorNodeSensorPowerSupplyFPNanoBat() {}
    SensorNodeSensorPowerSupplyFPNanoBat(SensorNodeSensorPowerSupplySTM32VCC* adc, uint16_t interval, uint8_t offset)
        : SensorNodeSensorDriver(2, 0x566d4441, 1, interval, offset), adc(adc) {}
    uint32_t readValue();

private:
    SensorNodeSensorPowerSupplySTM32VCC* adc;
};
