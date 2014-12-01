#include "global.h"
#include "device/bq24292i/bq24292i.h"
#include "sys/time.h"
#include "sys/util.h"


BQ24292I::BQ24292I(I2C::Bus* bus) : bus(bus)
{
    memset(&regs, 0, sizeof(regs));
    refresh();
}

bool BQ24292I::reset()
{
    uint8_t data = 0x80;
    bus->writeRegs(0x6b, 1, &data, 1);
    return refresh();
}

bool BQ24292I::refresh()
{
    return bus->readRegs(0x6b, 0, &regs, sizeof(regs)) == I2C::RESULT_OK;
}

struct BQ24292I::Status BQ24292I::getStatus()
{
    return regs.b.STATUS;
}

bool BQ24292I::setInput(class Input in)
{
    regs.b.EN_HIZ = in.EN_HIZ;
    regs.b.VINDPM = in.VINDPM;
    regs.b.IINLIM = in.IINLIM;
    regs.b.SYS_MIN = in.SYS_MIN;
    return bus->writeRegs(0x6b, 0, &regs, 2) == I2C::RESULT_OK;
}

bool BQ24292I::setOTG(bool on, bool highCurrent)
{
    regs.b.CHG_CONFIG_OTG = on;
    regs.b.BOOST_LIM = (typeof(regs.b.BOOST_LIM))highCurrent;
    return bus->writeRegs(0x6b, 1, &regs.d8[1], 1) == I2C::RESULT_OK;
}

bool BQ24292I::setBattery(class Battery bat)
{
    regs.b.ICHG = bat.ICHG;
    regs.b.FORCE_20PCT = false;
    regs.b.IPRECHG = bat.IPRECHG;
    regs.b.ITERM = bat.ITERM;
    regs.b.VREG = bat.VREG;
    regs.b.BATLOWV = bat.BATLOWV;
    regs.b.VRECHG = bat.VRECHG;
    regs.b.EN_TERM = bat.EN_TERM;
    regs.b.TERM_STAT = false;
    regs.b.BAT_COMP = bat.BAT_COMP;
    regs.b.VCLAMP = bat.VCLAMP;
    regs.b.TREG = bat.TREG;
    regs.b.BATFET_DISABLE = bat.BATFET_DISABLE;
    return bus->writeRegs(0x6b, 2, &regs.d8[2], 6) == I2C::RESULT_OK;
}

bool BQ24292I::enableCharging(bool on)
{
    regs.b.CHG_CONFIG_CHARGE = on;
    return bus->writeRegs(0x6b, 1, &regs.d8[1], 1) == I2C::RESULT_OK;
}

bool BQ24292I::setMonitoring(class Monitoring monitoring)
{
    regs.b.WATCHDOG = monitoring.WATCHDOG;
    regs.b.EN_TIMER = monitoring.EN_TIMER;
    regs.b.CHG_TIMER = monitoring.CHG_TIMER;
    regs.b.TMR2X_EN = monitoring.TMR2X_EN;
    regs.b.CHRG_FAULT_IRQE = monitoring.CHRG_FAULT_IRQE;
    regs.b.BAT_FAULT_IRQE = monitoring.BAT_FAULT_IRQE;
    return bus->writeRegs(0x6b, 6, &regs.d8[6], 3) == I2C::RESULT_OK;
}

bool BQ24292I::resetWatchdog()
{
    regs.b.WDRESET = true;
    return bus->writeRegs(0x6b, 1, &regs.d8[1], 1) == I2C::RESULT_OK;
}
