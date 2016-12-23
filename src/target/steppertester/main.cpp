#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/timer_regs.h"
#include "stepper.h"
#include "storage.h"
#include "lcd.h"
#include "font.h"
#include "button.h"
#include "menu.h"


struct __attribute__((packed, aligned(4))) IntParameter
{
    volatile int* ptr;
    int min;
    int max;
    uint16_t stepSize;
    uint8_t repeatInterval;
    uint8_t repeatFactor;
    constexpr IntParameter(volatile int* ptr, int min, int max, uint16_t stepSize,
                           uint8_t repeatInterval, uint8_t repeatFactor)
        : ptr(ptr), min(min), max(max), stepSize(stepSize), repeatInterval(repeatInterval), repeatFactor(repeatFactor) {}
};

static void intParamRenderer(void* arg, int row)
{
    IntParameter* param = (IntParameter*)arg;
    lcd.printf(row, 55, font, 0, "%6d", *param->ptr);
}


static int intParamHandler(void* arg, int row)
{
    configChanged = true;
    IntParameter* param = (IntParameter*)arg;
    int timeout = TIMEOUT_SETUP(60000000);
    int factor = 1;
    while (true)
    {
        int value = *param->ptr;
        lcd.printf(row, 55, font, 0xff, "%6d", value);
        uint32_t event = waitButtonRepeat(param->repeatInterval << 10);
        if (event & 6)
        {
            if (event & 2) *param->ptr = MAX(param->min, value - param->stepSize * factor);
            else *param->ptr = MIN(param->max, value + param->stepSize * factor);
            timeout = TIMEOUT_SETUP(60000000);
            factor = param->repeatFactor;
        }
        else if (event) return -1;
        else if (TIMEOUT_EXPIRED(timeout)) return -2;
        else factor = 1;
    }
}


static const char* driverName[] = {"A4988", "DRV8825"};


static void driverRenderer(void* arg, int row)
{
    lcd.printf(row, 49, font, 0, "%7s", driverName[cfg.data.driver]);
}


static int driverHandler(void* arg, int row)
{
    configChanged = true;
    while (true)
    {
        lcd.printf(row, 49, font, 0xff, "%7s", driverName[cfg.data.driver]);
        uint32_t event = waitButton(60000000);
        if ((event & 2) && cfg.data.driver) cfg.data.driver--;
        else if ((event & 4) && cfg.data.driver < ARRAYLEN(driverName) - 1) cfg.data.driver++;
        if (event & 6) setMicrostepping(cfg.data.microstepping);
        else return event ? -1 : -2;
    }
}


static void microsteppingRenderer(void* arg, int row)
{
    lcd.printf(row, 69, font, 0, "%2dx", 1 << cfg.data.microstepping);
}


static int microsteppingHandler(void* arg, int row)
{
    configChanged = true;
    while (true)
    {
        lcd.printf(row, 69, font, 0xff, "%2dx", 1 << cfg.data.microstepping);
        uint32_t event = waitButton(60000000);
        if ((event & 2) && cfg.data.microstepping) setMicrostepping(cfg.data.microstepping - 1);
        else if (event & 4) setMicrostepping(cfg.data.microstepping + 1);
        else return event ? -1 : -2;
    }
}


static void wiggleRenderer(void* arg, int row)
{
    lcd.printf(row, 70, font, 0, "%3s", wiggle ? "On": "Off");
}


static int wiggleHandler(void* arg, int row)
{
    wiggle ^= 1;
    return -1;
}


static int slipHandler()
{
    wiggle = false;
    targetpos = 0;
    pos = 0;
    lcd.clear();
    lcd.print(0, 0, font, 0x80, "Slip mode");
    lcd.print(4, 0, font, 0, "Press center key");
    lcd.print(5, 0, font, 0, "to exit slip mode.");
    int strength = 100;
    STM32_TIM14_REGS.CCR1 = strength * 480;
    union STM32_TIM_REG_TYPE::CCMR1 ccmr1 = { 0 };
    ccmr1 = { 0 };
    ccmr1.oc.OC1M = 6;
    STM32_TIM14_REGS.CCMR1.d32 = ccmr1.d32;
    while (waitButtonRepeat(0));
    while (true)
    {
        lcd.print(2, 0, font, 0, "Holding strength:");
        lcd.printf(3, 70, font, 0, "%3d", strength);
        uint32_t event = waitButtonRepeat(20000);
        if (event & 1) break;
        else if ((event & 2) && strength) strength--;
        else if ((event & 4) && strength < 100) strength++;
        STM32_TIM14_REGS.CCR1 = strength * 480;
    }
    ccmr1.oc.OC1M = 5;
    STM32_TIM14_REGS.CCMR1.d32 = ccmr1.d32;
    return -1;
}


static int floatHandler()
{
    wiggle = false;
    targetpos = 0;
    pos = 0;
    lcd.clear();
    lcd.print(2, 0, font, 0, "Press any key to");
    lcd.print(3, 0, font, 0, "exit float mode.");
    union STM32_TIM_REG_TYPE::CCMR1 ccmr1 = { 0 };
    ccmr1 = { 0 };
    ccmr1.oc.OC1M = 4;
    STM32_TIM14_REGS.CCMR1.d32 = ccmr1.d32;
    while (!waitButton(60000000));
    ccmr1.oc.OC1M = 5;
    STM32_TIM14_REGS.CCMR1.d32 = ccmr1.d32;
    return -1;
}


static int measureHandler()
{
    wiggle = false;
    targetpos = 0;
    pos = 0;
    lcd.clear();
    lcd.print(0, 0, font, 0x80, "Measure position");
    lcd.print(4, 0, font, 0, "Press center key");
    lcd.print(5, 0, font, 0, "to exit meas mode");
    while (waitButtonRepeat(0));
    int speed = 0;
    while (true)
    {
        lcd.printf(2, 0, font, 0, "Position: %+8d", pos);
        uint32_t event = getButtonState();
        speed = MIN(300, speed + 5);
        if (event & 1) break;
        else if (event & 2) targetpos = pos - speed;
        else if (event & 4) targetpos = pos + speed;
        else speed = 0;
        udelay(10000);
    }
    waitButton(0);
    cfg.data.wiggleTo = pos;
    targetpos = 0;
    configChanged = true;
    return -1;
}


static const IntParameter distanceParam(&cfg.data.wiggleTo, 0, 999999, 10, 10, 10);
static const IntParameter maxSpeedParam(&cfg.data.maxSpeed, 0, 200000, 1, 10, 10);
static const IntParameter accelFwdParam(&cfg.data.accelFwd, 0, 5000, 1, 10, 1);
static const IntParameter brakeFwdParam(&cfg.data.brakeFwd, 0, 10000, 1, 10, 1);
static const IntParameter accelBwdParam(&cfg.data.accelBwd, 0, 5000, 1, 10, 1);
static const IntParameter brakeBwdParam(&cfg.data.brakeBwd, 0, 10000, 1, 10, 1);


static const struct __attribute__((packed, aligned(4))) ParameterMenu
{
    Menu header;
    MenuEntry entry[9];
} parameterMenu =
{
    Menu(0, 6, 60, ARRAYLEN(ParameterMenu::entry), "Parameters"),
    {
        MenuEntry("Back to main menu"),
        MenuEntry("Driver", driverRenderer, driverHandler, NULL),
        MenuEntry("Microstepping", microsteppingRenderer, microsteppingHandler, NULL),
        MenuEntry("Distance", intParamRenderer, intParamHandler, const_cast<void*>((void*)&distanceParam)),
        MenuEntry("Max Speed", intParamRenderer, intParamHandler, const_cast<void*>((void*)&maxSpeedParam)),
        MenuEntry("Fwd Accel", intParamRenderer, intParamHandler, const_cast<void*>((void*)&accelFwdParam)),
        MenuEntry("Fwd Brake", intParamRenderer, intParamHandler, const_cast<void*>((void*)&brakeFwdParam)),
        MenuEntry("Bwd Accel", intParamRenderer, intParamHandler, const_cast<void*>((void*)&accelBwdParam)),
        MenuEntry("Bwd Brake", intParamRenderer, intParamHandler, const_cast<void*>((void*)&brakeBwdParam)),
    }
};

static const struct __attribute__((packed, aligned(4))) MainMenu
{
    Menu header;
    MenuEntry entry[6];
} mainMenu =
{
    Menu(0, 6, 60, ARRAYLEN(MainMenu::entry), "Main menu"),
    {
        MenuEntry("Status"),
        MenuEntry("Parameters", &parameterMenu.header),
        MenuEntry("Move fwd/bwd", wiggleRenderer, wiggleHandler, NULL),
        MenuEntry("Slip mode", slipHandler),
        MenuEntry("Float mode", floatHandler),
        MenuEntry("Measure position", measureHandler),
    }
};


int main()
{
    GPIO::enableFast(PIN_A0, true);

    initStorage();
    initButton();
    initStepper();

    while (true)
    {
        drawStatus();
        uint32_t event = waitButton(10000);
        if (event & 1)
        {
            showMenu(&mainMenu.header);
            saveConfig();
            lcd.clear();
        }
    }
}
