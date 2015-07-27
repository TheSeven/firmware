#include "global.h"
#include "app/sensornode/sensor/powersupply/fpnano.h"
#include "interface/clockgate/clockgate.h"
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/f0/adc_regs.h"
#include "soc/stm32/gpio.h"


uint32_t SENSORNODE_SENSOR_OPTIMIZE SensorNodeSensorPowerSupplyFPNano::readValue()
{
    union
    {
        uint32_t u32;
        struct
        {
            uint16_t systemVoltage;
            uint16_t batteryVoltage;
        } f;
    } data;

    clockgate_enable(STM32_ADC_CLOCKGATE, true);
    union STM32_ADC_COMMON_REG_TYPE::CCR CCR = { 0 };
    CCR.b.TSEN = true;
    CCR.b.VREFEN = true;
    STM32_ADC_REGS.CCR.d32 = CCR.d32;
    union STM32_ADC_REG_TYPE::CFGR1 CFGR1 = { 0 };
    STM32_ADC1_REGS.CFGR1.d32 = CFGR1.d32;
    union STM32_ADC_REG_TYPE::CFGR2 CFGR2 = { 0 };
    STM32_ADC1_REGS.CFGR2.d32 = CFGR2.d32;
    union STM32_ADC_REG_TYPE::CR CR = { 0 };
    CR.b.ADCAL = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    while (STM32_ADC1_REGS.CR.b.ADCAL);
    CR.d32 = 0;
    CR.b.ADEN = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    while (!STM32_ADC1_REGS.ISR.b.ADRDY);
    STM32_ADC1_REGS.SMPR = 3;
    STM32_ADC1_REGS.CHSELR.d32 = 1 << 17;
    CR.b.ADSTART = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    while (!STM32_ADC1_REGS.ISR.b.EOC);
    uint16_t adRef = STM32_ADC1_REGS.DR;
    STM32_ADC1_REGS.CHSELR.d32 = 1 << SENSORNODE_SENSOR_POWERSUPPLY_FPNANO_ADCPORT;
    CR.b.ADSTART = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    uint32_t system = (3300 << 8) * *((uint16_t*)0x1ffff7ba) / adRef;
    data.f.systemVoltage = system >> 8;
    while (!STM32_ADC1_REGS.ISR.b.EOC);
    uint16_t adBatt = STM32_ADC1_REGS.DR;
    data.f.batteryVoltage = (adBatt * system) >> 20;
    CR.d32 = 0;
    CR.b.ADDIS = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    STM32_ADC_REGS.CCR.d32 = 0;
    while (STM32_ADC1_REGS.CR.b.ADEN);
    clockgate_enable(STM32_ADC_CLOCKGATE, true);
    return data.u32;
}
