#pragma once

#include "global.h"
#include "interface/i2c/i2c.h"
#include "sys/util.h"


class __attribute__((packed,aligned(4))) BQ24292I final
{
public:
    class __attribute__((packed,aligned(4))) Status
    {
    public:
        bool batteryBelowSystemVoltage : 1;
        bool dieTemperatureLimiting : 1;
        bool inputPowerGood : 1;
        bool inputPowerLimiting : 1;
        enum ChargingState
        {
            NotCharging = 0,
            PreCharging = 1,
            FastCharging = 2,
            ChargingDone = 3,
        } chargingState : 2;
        enum PowerSource
        {
            UnknownSource = 0,
            USBHost = 1,
            ExternalPower = 2,
            SourcingOTG = 3,
        } powerSource : 2;
        enum ThermistorFault
        {
            ThermistorNormal = 0,
            Thermistor1Cold = 1,
            Thermistor1Hot = 2,
            Thermistor2Cold = 3,
            Thermistor2Hot = 4,
            ThermistorBothCold = 5,
            ThermistorBothHot = 6,
        } thermistorFault : 3;
        bool batteryOvervoltage : 1;
        enum ChargerFault
        {
            ChargerNormal = 0,
            InputVoltageFault = 1,
            DieOverheated = 2,
            SafetyTimerExpired = 3,
        } chargerFault : 2;
        bool otgFault : 1;
        bool watchdogExpired : 1;
    };

    enum IINLIM
    {
        IINLIM_100MA = 0,
        IINLIM_150MA = 1,
        IINLIM_500MA = 2,
        IINLIM_900MA = 3,
        IINLIM_1200MA = 4,
        IINLIM_1500MA = 5,
        IINLIM_2000MA = 6,
        IINLIM_3000MA = 7,
    };

    enum VINDPM
    {
        VINDPM_3880MV = 0x0,
        VINDPM_3960MV = 0x1,
        VINDPM_4040MV = 0x2,
        VINDPM_4120MV = 0x3,
        VINDPM_4200MV = 0x4,
        VINDPM_4280MV = 0x5,
        VINDPM_4360MV = 0x6,
        VINDPM_4440MV = 0x7,
        VINDPM_4520MV = 0x8,
        VINDPM_4600MV = 0x9,
        VINDPM_4680MV = 0xa,
        VINDPM_4760MV = 0xb,
        VINDPM_4840MV = 0xc,
        VINDPM_4920MV = 0xd,
        VINDPM_5000MV = 0xe,
        VINDPM_5080MV = 0xf,
    };

    enum SYS_MIN
    {
        SYS_MIN_3000MV = 0,
        SYS_MIN_3100MV = 1,
        SYS_MIN_3200MV = 2,
        SYS_MIN_3300MV = 3,
        SYS_MIN_3400MV = 4,
        SYS_MIN_3500MV = 5,
        SYS_MIN_3600MV = 6,
        SYS_MIN_3700MV = 7,
    };

    enum ICHG
    {
        ICHG_512MA = 0x00,
        ICHG_576MA = 0x01,
        ICHG_640MA = 0x02,
        ICHG_704MA = 0x03,
        ICHG_768MA = 0x04,
        ICHG_832MA = 0x05,
        ICHG_896MA = 0x06,
        ICHG_960MA = 0x07,
        ICHG_1024MA = 0x08,
        ICHG_1088MA = 0x09,
        ICHG_1152MA = 0x0a,
        ICHG_1216MA = 0x0b,
        ICHG_1280MA = 0x0c,
        ICHG_1344MA = 0x0d,
        ICHG_1408MA = 0x0e,
        ICHG_1472MA = 0x0f,
        ICHG_1536MA = 0x10,
        ICHG_1600MA = 0x11,
        ICHG_1664MA = 0x12,
        ICHG_1728MA = 0x13,
        ICHG_1792MA = 0x14,
        ICHG_1856MA = 0x15,
        ICHG_1920MA = 0x16,
        ICHG_1984MA = 0x17,
        ICHG_2048MA = 0x18,
        ICHG_2112MA = 0x19,
        ICHG_2176MA = 0x1a,
        ICHG_2240MA = 0x1b,
        ICHG_2304MA = 0x1c,
        ICHG_2368MA = 0x1d,
        ICHG_2432MA = 0x1e,
        ICHG_2496MA = 0x1f,
        ICHG_2560MA = 0x20,
        ICHG_2624MA = 0x21,
        ICHG_2688MA = 0x22,
        ICHG_2752MA = 0x23,
        ICHG_2816MA = 0x24,
        ICHG_2880MA = 0x25,
        ICHG_2944MA = 0x26,
        ICHG_3008MA = 0x27,
        ICHG_3072MA = 0x28,
        ICHG_3136MA = 0x29,
        ICHG_3200MA = 0x2a,
        ICHG_3264MA = 0x2b,
        ICHG_3328MA = 0x2c,
        ICHG_3392MA = 0x2d,
        ICHG_3456MA = 0x2e,
        ICHG_3520MA = 0x2f,
        ICHG_3584MA = 0x30,
        ICHG_3648MA = 0x31,
        ICHG_3712MA = 0x32,
        ICHG_3776MA = 0x33,
        ICHG_3840MA = 0x34,
        ICHG_3904MA = 0x35,
        ICHG_3968MA = 0x36,
        ICHG_4032MA = 0x37,
        ICHG_4096MA = 0x38,
        ICHG_4160MA = 0x39,
        ICHG_4224MA = 0x3a,
        ICHG_4288MA = 0x3b,
        ICHG_4352MA = 0x3c,
        ICHG_4416MA = 0x3d,
        ICHG_4480MA = 0x3e,
        ICHG_4544MA = 0x3f,
    };

    enum ITERM
    {
        ITERM_128MA = 0x0,
        ITERM_256MA = 0x1,
        ITERM_384MA = 0x2,
        ITERM_512MA = 0x3,
        ITERM_640MA = 0x4,
        ITERM_768MA = 0x5,
        ITERM_896MA = 0x6,
        ITERM_1024MA = 0x7,
        ITERM_1152MA = 0x8,
        ITERM_1280MA = 0x9,
        ITERM_1408MA = 0xa,
        ITERM_1536MA = 0xb,
        ITERM_1664MA = 0xc,
        ITERM_1792MA = 0xd,
        ITERM_1920MA = 0xe,
        ITERM_2048MA = 0xf,
    };

    enum IPRECHG
    {
        IPRECHG_128MA = 0x0,
        IPRECHG_256MA = 0x1,
        IPRECHG_384MA = 0x2,
        IPRECHG_512MA = 0x3,
        IPRECHG_640MA = 0x4,
        IPRECHG_768MA = 0x5,
        IPRECHG_896MA = 0x6,
        IPRECHG_1024MA = 0x7,
        IPRECHG_1152MA = 0x8,
        IPRECHG_1280MA = 0x9,
        IPRECHG_1408MA = 0xa,
        IPRECHG_1536MA = 0xb,
        IPRECHG_1664MA = 0xc,
        IPRECHG_1792MA = 0xd,
        IPRECHG_1920MA = 0xe,
        IPRECHG_2048MA = 0xf,
    };

    enum VRECHG
    {
        VRECHG_100MV = 0,
        VRECHG_300MV = 1,
    };

    enum BATLOWV
    {
        BATLOWV_2800MV = 0,
        BATLOWV_3000MV = 1,
    };

    enum VREG
    {
        VREG_3504MV = 0x00,
        VREG_3520MV = 0x01,
        VREG_3536MV = 0x02,
        VREG_3552MV = 0x03,
        VREG_3568MV = 0x04,
        VREG_3584MV = 0x05,
        VREG_3600MV = 0x06,
        VREG_3616MV = 0x07,
        VREG_3632MV = 0x08,
        VREG_3648MV = 0x09,
        VREG_3664MV = 0x0a,
        VREG_3680MV = 0x0b,
        VREG_3696MV = 0x0c,
        VREG_3712MV = 0x0d,
        VREG_3728MV = 0x0e,
        VREG_3744MV = 0x0f,
        VREG_3760MV = 0x10,
        VREG_3776MV = 0x11,
        VREG_3792MV = 0x12,
        VREG_3808MV = 0x13,
        VREG_3824MV = 0x14,
        VREG_3840MV = 0x15,
        VREG_3856MV = 0x16,
        VREG_3872MV = 0x17,
        VREG_3888MV = 0x18,
        VREG_3904MV = 0x19,
        VREG_3920MV = 0x1a,
        VREG_3936MV = 0x1b,
        VREG_3952MV = 0x1c,
        VREG_3968MV = 0x1d,
        VREG_3984MV = 0x1e,
        VREG_4000MV = 0x1f,
        VREG_4016MV = 0x20,
        VREG_4032MV = 0x21,
        VREG_4048MV = 0x22,
        VREG_4064MV = 0x23,
        VREG_4080MV = 0x24,
        VREG_4096MV = 0x25,
        VREG_4112MV = 0x26,
        VREG_4128MV = 0x27,
        VREG_4144MV = 0x28,
        VREG_4160MV = 0x29,
        VREG_4176MV = 0x2a,
        VREG_4192MV = 0x2b,
        VREG_4208MV = 0x2c,
        VREG_4224MV = 0x2d,
        VREG_4240MV = 0x2e,
        VREG_4256MV = 0x2f,
        VREG_4272MV = 0x30,
        VREG_4288MV = 0x31,
        VREG_4304MV = 0x32,
        VREG_4320MV = 0x33,
        VREG_4336MV = 0x34,
        VREG_4352MV = 0x35,
        VREG_4368MV = 0x36,
        VREG_4384MV = 0x37,
        VREG_4400MV = 0x38,
        VREG_4416MV = 0x39,
        VREG_4432MV = 0x3a,
        VREG_4448MV = 0x3b,
        VREG_4464MV = 0x3c,
        VREG_4480MV = 0x3d,
        VREG_4496MV = 0x3e,
        VREG_4512MV = 0x3f,
    };

    enum CHG_TIMER
    {
        CHG_TIMER_5H = 0,
        CHG_TIMER_8H = 1,
        CHG_TIMER_12H = 2,
        CHG_TIMER_20H = 3,
    };

    enum WATCHDOG
    {
        WATCHDOG_DISABLE = 0,
        WATCHDOG_40S = 1,
        WATCHDOG_80S = 2,
        WATCHDOG_160S = 3,
    };

    enum TREG
    {
        TREG_60C = 0,
        TREG_80C = 1,
        TREG_100C = 2,
        TREG_120C = 3,
    };

    enum VCLAMP
    {
        VCLAMP_0MV = 0,
        VCLAMP_16MV = 1,
        VCLAMP_32MV = 2,
        VCLAMP_48MV = 3,
        VCLAMP_64MV = 4,
        VCLAMP_80MV = 5,
        VCLAMP_96MV = 6,
        VCLAMP_112MV = 7,
    };

    enum BAT_COMP
    {
        BAT_COMP_0MR = 0,
        BAT_COMP_10MR = 1,
        BAT_COMP_20MR = 2,
        BAT_COMP_30MR = 3,
        BAT_COMP_40MR = 4,
        BAT_COMP_50MR = 5,
        BAT_COMP_60MR = 6,
        BAT_COMP_70MR = 7,
    };

    class __attribute__((packed,aligned(4))) Input
    {
    public:
        bool EN_HIZ : 1;
        enum VINDPM VINDPM : 4;
        enum IINLIM IINLIM : 3;
        enum SYS_MIN SYS_MIN : 3;

        constexpr Input() : EN_HIZ(false), VINDPM(VINDPM_4440MV), IINLIM(IINLIM_500MA), SYS_MIN(SYS_MIN_3500MV) {}

        inline Input(int minInVoltage, int maxInCurrent, int minSysVoltage) : Input()
        {
            setMinInVoltage(minInVoltage);
            setMaxInCurrent(maxInCurrent);
            setMinSysVoltage(minSysVoltage);
        }

        inline void disconnect(bool disconnect)
        {
            EN_HIZ = disconnect;
        }

        inline void setMinInVoltage(int voltage)
        {
            VINDPM = (enum VINDPM)MAX(0, MIN(15, (voltage - 3880000) / 80000));
        }

        inline void setMaxInCurrent(int current)
        {
            switch (current)
            {
            case 0 ... 149999: IINLIM = IINLIM_100MA; break;
            case 150000 ... 499999: IINLIM = IINLIM_150MA; break;
            case 500000 ... 899999: IINLIM = IINLIM_500MA; break;
            case 900000 ... 1199999: IINLIM = IINLIM_900MA; break;
            case 1200000 ... 1499999: IINLIM = IINLIM_1200MA; break;
            case 1500000 ... 1999999: IINLIM = IINLIM_1500MA; break;
            case 2000000 ... 2999999: IINLIM = IINLIM_2000MA; break;
            default: IINLIM = IINLIM_3000MA; break;
            }
        }

        inline void setMinSysVoltage(int voltage)
        {
            SYS_MIN = (enum SYS_MIN)MAX(0, MIN(7, (voltage - 3000000) / 100000));
        }
    };

    class __attribute__((packed,aligned(4))) Battery
    {
    public:
        enum VREG VREG : 6;
        enum BATLOWV BATLOWV : 1;
        enum VRECHG VRECHG : 1;
        enum ICHG ICHG : 6;
        enum IPRECHG IPRECHG : 4;
        enum ITERM ITERM : 4;
        uint8_t EN_TERM : 1;
        uint8_t BATFET_DISABLE : 1;
        enum BAT_COMP BAT_COMP : 3;
        enum VCLAMP VCLAMP : 3;
        enum TREG TREG : 2;

        constexpr Battery()
            : VREG(VREG_4160MV), BATLOWV(BATLOWV_3000MV), VRECHG(VRECHG_100MV), ICHG(ICHG_512MA),
              IPRECHG(IPRECHG_128MA), ITERM(ITERM_128MA), EN_TERM(true), BATFET_DISABLE(false),
              BAT_COMP(BAT_COMP_0MR), VCLAMP(VCLAMP_0MV), TREG(TREG_80C) {}

        inline Battery(int chargingVoltage, int chargingCurrent, int innerResistance, int overdriveLimit) : Battery()
        {
            setChargingVoltage(chargingVoltage);
            setChargingCurrent(chargingCurrent);
            setInnerResistance(innerResistance);
            setOverdriveLimit(overdriveLimit);
        }

        inline void setChargingVoltage(int voltage)
        {
            VREG = (enum VREG)MAX(0, MIN(0x3f, (voltage - 3504000) / 16000));
        }

        inline void setPrechargeThreshold(int voltage)
        {
            BATLOWV = voltage < 3000000 ? BATLOWV_2800MV : BATLOWV_3000MV;
        }

        inline void setRechargeThreshold(int voltage)
        {
            VRECHG = voltage <= 100000 ? VRECHG_100MV : VRECHG_300MV;
        }

        inline void setChargingCurrent(int current)
        {
            ICHG = (enum ICHG)MAX(0, MIN(0x3f, (current - 512000) / 64000));
        }

        inline void setPrechargeCurrent(int current)
        {
            IPRECHG = (enum IPRECHG)MAX(0, MIN(15, (current - 128000) / 128000));
        }

        inline void setTerminationCurrent(int current)
        {
            ITERM = (enum ITERM)MAX(0, MIN(15, (current - 128000) / 128000));
        }

        inline void enableChargeTermination(bool on)
        {
            EN_TERM = on;
        }

        inline void disconnect(bool disconnect)
        {
            BATFET_DISABLE = disconnect;
        }

        inline void setInnerResistance(int resistance)
        {
            BAT_COMP = (enum BAT_COMP)MAX(0, MIN(7, resistance / 10000));
        }

        inline void setOverdriveLimit(int voltage)
        {
            VCLAMP = (enum VCLAMP)MAX(0, MIN(7, voltage / 16000));
        }

        inline void setDieTemperatureLimit(int temperature)
        {
            TREG = (enum TREG)MAX(0, MIN(3, (temperature - 80) / 20));
        }
    };

    class __attribute__((packed,aligned(4))) Monitoring
    {
    public:
        enum CHG_TIMER CHG_TIMER : 2;
        uint8_t EN_TIMER : 1;
        uint8_t TMR2X_EN : 1;
        uint8_t BAT_FAULT_IRQE : 1;
        uint8_t CHRG_FAULT_IRQE : 1;
        enum WATCHDOG WATCHDOG : 2;

        constexpr Monitoring()
            : CHG_TIMER(CHG_TIMER_8H), EN_TIMER(true), TMR2X_EN(true),
              BAT_FAULT_IRQE(true), CHRG_FAULT_IRQE(true), WATCHDOG(WATCHDOG_DISABLE) {}

        inline void setChargingTimeLimit(bool enable, int hours, bool stretchDuringThrottling)
        {
            EN_TIMER = enable;
            TMR2X_EN = stretchDuringThrottling;
            switch (hours)
            {
            case 0 ... 7: CHG_TIMER = CHG_TIMER_5H; break;
            case 8 ... 11: CHG_TIMER = CHG_TIMER_8H; break;
            case 12 ... 19: CHG_TIMER = CHG_TIMER_12H; break;
            default: CHG_TIMER = CHG_TIMER_20H; break;
            }
        }

        inline void enableBatFaultIRQ(bool on)
        {
            BAT_FAULT_IRQE = on;
        }

        inline void enableChargerFaultIRQ(bool on)
        {
            CHRG_FAULT_IRQE = on;
        }

        inline void setWatchdogTimer(int seconds)
        {
            switch (seconds)
            {
            case 0 ... 40: WATCHDOG = WATCHDOG_40S; break;
            case 41 ... 80: WATCHDOG = WATCHDOG_80S; break;
            case 81 ... 160: WATCHDOG = WATCHDOG_160S; break;
            default: WATCHDOG = WATCHDOG_DISABLE; break;
            }
        }
    };

    BQ24292I(I2C::Bus* bus);
    bool reset();
    bool refresh();
    struct Status getStatus();
    bool setInput(class Input in);
    bool setOTG(bool on, bool highCurrent);
    bool setBattery(class Battery bat);
    bool enableCharging(bool on);
    bool setMonitoring(class Monitoring monitoring);
    bool resetWatchdog();

private:
    I2C::Bus* bus;
    union __attribute__((packed)) Registers
    {
        uint8_t d8[11];
        struct __attribute__((packed)) REGS
        {
            enum IINLIM IINLIM : 3;
            enum VINDPM VINDPM : 4;
            uint8_t EN_HIZ : 1;
            enum BOOST_LIM
            {
                BOOST_LIM_500MA = 0,
                BOOST_LIM_1300MA = 1,
            } BOOST_LIM : 1;
            enum SYS_MIN SYS_MIN : 3;
            uint8_t CHG_CONFIG_CHARGE : 1;
            uint8_t CHG_CONFIG_OTG : 1;
            uint8_t WDRESET : 1;
            uint8_t RESET : 1;
            uint8_t FORCE_20PCT : 1;
            uint8_t : 1;
            enum ICHG ICHG : 6;
            enum ITERM ITERM : 4;
            enum IPRECHG IPRECHG : 4;
            enum VRECHG VRECHG : 1;
            enum BATLOWV BATLOWV : 1;
            enum VREG VREG : 6;
            uint8_t : 1;
            enum CHG_TIMER CHG_TIMER : 2;
            uint8_t EN_TIMER : 1;
            enum WATCHDOG WATCHDOG : 2;
            uint8_t TERM_STAT : 1;
            uint8_t EN_TERM : 1;
            enum TREG TREG : 2;
            enum VCLAMP VCLAMP : 3;
            enum BAT_COMP BAT_COMP : 3;
            uint8_t BAT_FAULT_IRQE : 1;
            uint8_t CHRG_FAULT_IRQE : 1;
            uint8_t : 3;
            uint8_t BATFET_DISABLE : 1;
            uint8_t TMR2X_EN : 1;
            uint8_t DPDM_EN : 1;
            struct Status STATUS;
            uint8_t DEV_REG : 2;
            uint8_t TS_PROFILE : 1;
            uint8_t PN : 3;
            uint8_t : 2;
        } b;
    } regs;
};
