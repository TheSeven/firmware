#pragma once

#include "global.h"
#include "interface/i2c/i2c.h"


class __attribute__((packed,aligned(4))) __attribute__((packed)) STC3105 final
{
public:
    enum Mode
    {
        MODE_STANDBY = 0,
        MODE_POWERSAVE,
        MODE_ACTIVE,
    };

    struct Alarms
    {
        bool lowCharge : 1;
        bool lowVoltage : 1;
        bool powerLost : 1;
    };

    STC3105(I2C::Bus* bus, int shuntConductance);
    bool reset();
    bool refresh();
    struct Alarms getAlarms();
    int getVoltage();
    int getCurrent();
    int getCharge();
    int getConversionCounter();
    int getRelaxationCounter();
    bool clearAlarms();
    bool setMode(enum Mode mode);
    bool setCharge(int charge);
    bool setAlarmCharge(int charge);
    bool setAlarmVoltage(int voltage);
    bool enableAlarm(bool on);
    bool setRelaxationThreshold(int current);


    union __attribute__((packed)) Registers
    {
        uint8_t d8[17];
        struct __attribute__((packed)) REGS
        {
            uint8_t : 2;
            bool PWR_SAVE : 1;
            bool ALM_ENA : 1;
            bool GG_RUN : 1;
            uint8_t : 3;
            bool IO0DATA : 1;
            bool GG_RST : 1;
            bool GG_EOC : 1;
            bool VM_EOC : 1;
            bool PORDET : 1;
            bool ALM_SOC : 1;
            bool ALM_VOLT : 1;
            uint8_t : 1;
            int16_t CHARGE;
            uint16_t COUNTER;
            int16_t CURRENT;
            uint16_t VOLTAGE;
            int16_t SOC_BASE;
            int16_t ALARM_SOC;
            uint8_t ALARM_VOLTAGE;
            uint8_t CURRENT_THRES;
            uint8_t RELAX_COUNT;
        } b;
    } regs;

private:
    I2C::Bus* bus;
    int shuntConductance;

};
