#include "global.h"
#include "device/stc3105/stc3105.h"
#include "sys/time.h"
#include "sys/util.h"


STC3105::STC3105(I2C::Bus* bus, int shuntConductance) : bus(bus), shuntConductance(shuntConductance)
{
    refresh();
}

bool STC3105::reset()
{
    uint8_t data = 0x13;
    if (bus->writeRegs(0x70, 1, &data, 1) != I2C::RESULT_OK) return false;
    data = 3;
    if (bus->writeRegs(0x70, 1, &data, 1) != I2C::RESULT_OK) return false;
    return refresh();
}

bool STC3105::refresh()
{
    return bus->readRegs(0x70, 0, &regs, sizeof(regs)) == I2C::RESULT_OK;
}

struct STC3105::Alarms STC3105::getAlarms()
{
    return { regs.b.ALM_SOC, regs.b.ALM_VOLT, regs.b.PORDET };
}

int STC3105::getVoltage()
{
    return 2440 * regs.b.VOLTAGE;
}

int STC3105::getCurrent()
{
    return (int)(((shuntConductance * regs.b.CURRENT) * (0x100000000ull * 1177 / 100)) >> 32);
}

int STC3105::getCharge()
{
    return (int)(((shuntConductance * regs.b.CHARGE) * (0x100000000ull * 67 / 10)) >> 32);
}

int STC3105::getConversionCounter()
{
    return regs.b.COUNTER;
}

int STC3105::getRelaxationCounter()
{
    return regs.b.RELAX_COUNT;
}

bool STC3105::clearAlarms()
{
    uint8_t data = 1;
    return bus->writeRegs(0x70, 1, &data, 1) == I2C::RESULT_OK;
}

bool STC3105::setMode(enum Mode mode)
{
    regs.b.PWR_SAVE = mode != MODE_ACTIVE;
    regs.b.GG_RUN = mode != MODE_STANDBY;
    return bus->writeRegs(0x70, 0, &regs, 1) == I2C::RESULT_OK;
}

bool STC3105::setCharge(int charge)
{
    int16_t data = charge * 10 / (67 * shuntConductance);
    if (bus->writeRegs(0x70, 10, &data, 2) != I2C::RESULT_OK) return false;
    data = 3;
    return bus->writeRegs(0x70, 1, &data, 1) == I2C::RESULT_OK;
}

bool STC3105::setAlarmCharge(int charge)
{
    int16_t data = charge * 10 / (67 * shuntConductance);
    return bus->writeRegs(0x70, 12, &data, 2) == I2C::RESULT_OK;
}

bool STC3105::setAlarmVoltage(int voltage)
{
    uint8_t data = voltage / 19520;
    return bus->writeRegs(0x70, 14, &data, 1) == I2C::RESULT_OK;
}

bool STC3105::enableAlarm(bool on)
{
    regs.b.ALM_ENA = on;
    return bus->writeRegs(0x70, 0, &regs, 1) == I2C::RESULT_OK;
}

bool STC3105::setRelaxationThreshold(int current)
{
    uint8_t data = current / (320 * shuntConductance);
    return bus->writeRegs(0x70, 15, &data, 1) == I2C::RESULT_OK;
}
