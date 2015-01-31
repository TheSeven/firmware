#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "interface/irq/irq.h"
#include "interface/clockgate/clockgate.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "cpu/arm/cortexm/irq.h"
#include "soc/stm32/f0/clockgates.h"
#include "soc/stm32/f0/rcc_regs.h"
#include "soc/stm32/f0/rtc_regs.h"
#include "soc/stm32/f0/adc_regs.h"
#include "soc/stm32/exti_regs.h"
#include "soc/stm32/pwr_regs.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/gpio_regs.h"
#include "soc/stm32/timer_regs.h"
#include "soc/stm32/spi.h"
#include "device/ds1820/ds1820.h"
#include "device/sdhc_spi/sdhc_spi.h"
#include "lib/fatfs/fatfs.h"


OneWire::Bus onewire(PIN_SENSOR);
DS1820 sensor[MAX_SENSORS];
SDHC_SPI sd(&STM32::SPI::SPI1, PIN_SD_CS, SD_FREQ);
FRESULT mountResult;
FatFs::FileSystem fs(&sd, 0, "0:", &mountResult);
uint16_t rtcFrequency;
int16_t rtcDeviation;
uint16_t lastTime;
int timeOffset;

#ifdef HAVE_LCD
#include "device/pcd8544/pcd8544.h"
PCD8544 lcd(&STM32::SPI::SPI1, PIN_LCD_CS, LCD_FREQ, PIN_LCD_CD, LCD_BIAS, LCD_TC, LCD_VOP);
const uint8_t font[] =
{
    0x20, 0x6d,  // ' ' - 'm'
    0x00, 0x04, 0x05, 0x05, 0x05, 0x05, 0x0a, 0x0a,
    0x0a, 0x0d, 0x10, 0x13, 0x16, 0x18, 0x1c, 0x1d,
    0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
    0x40, 0x44, 0x48, 0x49, 0x49, 0x4d, 0x50, 0x54,
    0x58, 0x58, 0x5c, 0x60, 0x64, 0x68, 0x6c, 0x70,
    0x74, 0x78, 0x79, 0x7c, 0x80, 0x84, 0x89, 0x8e,
    0x92, 0x96, 0x9a, 0x9e, 0xa2, 0xa7, 0xab, 0xb0,
    0xb5, 0xba, 0xbf, 0xc4, 0xc6, 0xc9, 0xcb, 0xcb,
    0xce, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6,
    0xea, 0xee, 0xef, 0xf2, 0xf6, 0xf8, 0xfd,
    0x00, 0x00, 0x00, 0x00,  // ' '
    0x5f,  // '!'
     // '"'
     // '#'
     // '$'
    0x23, 0x13, 0x08, 0x64, 0x62,  // '%'
     // '&'
     // '''
    0x1c, 0x22, 0x41,  // '('
    0x41, 0x22, 0x1c,  // ')'
    0x14, 0x08, 0x14,  // '*'
    0x08, 0x1c, 0x08,  // '+'
    0x80, 0x40,  // ','
    0x08, 0x08, 0x08, 0x08,  // '-'
    0x40,  // '.'
    0x60, 0x1c, 0x03,  // '/'
    0x3e, 0x49, 0x45, 0x3e,  // '0'
    0x44, 0x42, 0x7f, 0x40,  // '1'
    0x62, 0x51, 0x49, 0x46,  // '2'
    0x22, 0x49, 0x49, 0x36,  // '3'
    0x1c, 0x12, 0x7f, 0x10,  // '4'
    0x27, 0x45, 0x45, 0x39,  // '5'
    0x3c, 0x4a, 0x49, 0x31,  // '6'
    0x71, 0x09, 0x05, 0x03,  // '7'
    0x36, 0x49, 0x49, 0x36,  // '8'
    0x26, 0x49, 0x49, 0x3e,  // '9'
    0x24,  // ':'
     // ';'
    0x08, 0x14, 0x22, 0x41,  // '<'
    0x14, 0x14, 0x14,  // '='
    0x41, 0x22, 0x14, 0x08,  // '>'
    0x02, 0x51, 0x09, 0x06,  // '?'
     // '@'
    0x7e, 0x11, 0x11, 0x7e,  // 'A'
    0x7f, 0x49, 0x49, 0x36,  // 'B'
    0x3e, 0x41, 0x41, 0x22,  // 'C'
    0x7f, 0x41, 0x22, 0x1c,  // 'D'
    0x7f, 0x49, 0x49, 0x41,  // 'E'
    0x7f, 0x09, 0x09, 0x01,  // 'F'
    0x3e, 0x41, 0x49, 0x3a,  // 'G'
    0x7f, 0x08, 0x08, 0x7f,  // 'H'
    0x7f,  // 'I'
    0x20, 0x40, 0x3f,  // 'J'
    0x7f, 0x14, 0x22, 0x41,  // 'K'
    0x7f, 0x40, 0x40, 0x40,  // 'L'
    0x7f, 0x02, 0x0c, 0x02, 0x7f,  // 'M'
    0x7f, 0x04, 0x08, 0x10, 0x7f,  // 'N'
    0x3e, 0x41, 0x41, 0x3e,  // 'O'
    0x7f, 0x09, 0x09, 0x06,  // 'P'
    0x3e, 0x41, 0x21, 0x5e,  // 'Q'
    0x7f, 0x19, 0x29, 0x46,  // 'R'
    0x26, 0x49, 0x49, 0x32,  // 'S'
    0x01, 0x01, 0x7f, 0x01, 0x01,  // 'T'
    0x3f, 0x40, 0x40, 0x3f,  // 'U'
    0x1f, 0x20, 0x40, 0x20, 0x1f,  // 'V'
    0x7f, 0x20, 0x18, 0x20, 0x7f,  // 'W'
    0x63, 0x14, 0x08, 0x14, 0x63,  // 'X'
    0x03, 0x04, 0x78, 0x04, 0x03,  // 'Y'
    0x61, 0x51, 0x49, 0x45, 0x43,  // 'Z'
    0x7f, 0x41,  // '['
    0x03, 0x1c, 0x60,  // '\'
    0x41, 0x7f,  // ']'
     // '^'
    0x40, 0x40, 0x40,  // '_'
     // '`'
    0x20, 0x54, 0x54, 0x78,  // 'a'
    0x7f, 0x44, 0x44, 0x38,  // 'b'
    0x38, 0x44, 0x44, 0x28,  // 'c'
    0x38, 0x44, 0x44, 0x7f,  // 'd'
    0x38, 0x54, 0x54, 0x18,  // 'e'
    0x08, 0x7e, 0x09, 0x02,  // 'f'
    0x98, 0xa4, 0xa4, 0x7c,  // 'g'
    0x7f, 0x04, 0x04, 0x78,  // 'h'
    0x7d,  // 'i'
    0x20, 0x40, 0x3d,  // 'j'
    0x7f, 0x10, 0x28, 0x44,  // 'k'
    0x3f, 0x40,  // 'l'
    0x7c, 0x04, 0x18, 0x04, 0x78,  // 'm'
    0x6e, 0x7d,  // 'n' - '}'
    0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c,
    0x20, 0x25, 0x2a, 0x2f, 0x33, 0x36, 0x39, 0x39,
    0x3c,
    0x7c, 0x08, 0x04, 0x78,  // 'n'
    0x38, 0x44, 0x44, 0x38,  // 'o'
    0xfc, 0x24, 0x24, 0x18,  // 'p'
    0x18, 0x24, 0x24, 0xfc,  // 'q'
    0x7c, 0x08, 0x04, 0x08,  // 'r'
    0x48, 0x54, 0x54, 0x24,  // 's'
    0x04, 0x3f, 0x44, 0x20,  // 't'
    0x3c, 0x40, 0x20, 0x7c,  // 'u'
    0x1c, 0x20, 0x40, 0x20, 0x1c,  // 'v'
    0x3c, 0x40, 0x38, 0x40, 0x3c,  // 'w'
    0x44, 0x28, 0x10, 0x28, 0x44,  // 'x'
    0x0c, 0x50, 0x50, 0x3c,  // 'y'
    0x64, 0x54, 0x4c,  // 'z'
    0x08, 0x36, 0x41,  // '{'
     // '|'
    0x41, 0x36, 0x08,  // '}'
    0xb0, 0xb0,  // '°' - '°'
    0x00, 0x03,
    0x02, 0x05, 0x02,  // '°'
    0, 0,
};
#endif

struct DataPoint
{
    uint32_t time;
    uint16_t batteryVoltage;
    uint16_t systemVoltage;
    int16_t chipTemp;
    int16_t sensor[MAX_SENSORS];
} dataBuffer[MAX_BUFFER];

struct Config
{
    uint16_t interval;
    uint16_t buffer;
    uint8_t sensors;
    uint8_t resolution;
} config;


bool OneWire::Bus::reset() const
{
    GPIO::setPull(pin, GPIO::PULL_UP);
    int time = TIMEOUT_SETUP(1000);
    while (!GPIO::getLevel(pin)) if (TIMEOUT_EXPIRED(time)) return false;
    time = delay(read_usec_timer() + 100);
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 0x10000 << (pin.pin & 0xf);
    time = delay(time + 480);
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 1 << (pin.pin & 0xf);
    time = delay(time + 70);
    bool result = !((STM32_GPIO_REGS(pin.pin >> 4).IDR.d32 >> (pin.pin & 0xf)) & 1);
    delay(time + 410);
    return result;
}


void OneWire::Bus::writeBit(bool bit) const
{
    int time = read_usec_timer();
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 0x10000 << (pin.pin & 0xf);
    time = delay(time + (bit ? 5 : 60));
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 1 << (pin.pin & 0xf);
    delay(time + (bit ? 55 : 5));
}


bool OneWire::Bus::readBit() const
{
    int time = read_usec_timer();
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 0x10000 << (pin.pin & 0xf);
    STM32_GPIO_REGS(pin.pin >> 4).BSRR.d32 = 1 << (pin.pin & 0xf);
    time = delay(time + 14);
    bool result = (STM32_GPIO_REGS(pin.pin >> 4).IDR.d32 >> (pin.pin & 0xf)) & 1;
    delay(time + 50);
    return result;
}


void DS1820::wait(int timeout)
{
    clockgate_enable(STM32_GPIO_CLOCKGATE(PIN_SENSOR.pin >> 4), false);
    idle();
    clockgate_enable(STM32_GPIO_CLOCKGATE(PIN_SENSOR.pin >> 4), true);
}


void __attribute__((noreturn)) error(uint32_t number)
{
#ifdef HAVE_LCD
    const static char* const errmsg[] =
    {
        "Unknown (%x)",
        "SD init",
        "SD mount",
        "config.txt open",
        "Too many sensors",
        "Sensor not found",
        "Log create",
        "Log write",
    };
    lcd.printf(2, 0, font, 0xff, "*** ERROR ***");
    int col = lcd.printf(3, 0, font, 0, errmsg[number > ARRAYLEN(errmsg) ? 0 : number], number);
    while (col < 84) col = lcd.printf(3, col, font, 0, " ");
#endif

    int interval = 5000000;
    while (true)
    {
        GPIO::setLevel(PIN_LED, true);
        udelay(1000000);
        GPIO::setLevel(PIN_LED, false);
        udelay(1000000);
        for (uint32_t i = 0; i < number; i++)
        {
            GPIO::setLevel(PIN_LED, true);
            udelay(50000);
            GPIO::setLevel(PIN_LED, false);
            udelay(300000);
        }
        interval = MIN(300000000, 11 * interval / 10);
        int timeout = TIMEOUT_SETUP(interval);
        while (!TIMEOUT_EXPIRED(timeout)) idle();
    }
}


bool skipUntil(FatFs::File* file, char delim)
{
    while (true)
    {
        char c;
        uint32_t read;
        if (file->read(&c, 1, &read) != FR_OK || read != 1) return false;
        if (c == delim) return true;
    }
}


int readUntil(FatFs::File* file, char* buffer, int maxlen, char delim)
{
    int len = 0;
    while (len < maxlen)
    {
        char c;
        uint32_t read;
        if (file->read(&c, 1, &read) != FR_OK || read != 1) return len;
        if (c == delim) return len;
        if (c == ' ' && !len) continue;
        if (c == '\r') continue;
        *buffer++ = c;
        len++;
    }
    skipUntil(file, delim);
    return -1;
}


bool readInt(FatFs::File* file, int* data, char delim)
{
    *data = 0;
    bool ok = false;
    while (true)
    {
        char c;
        uint32_t read;
        if (file->read(&c, 1, &read) != FR_OK || read != 1) return ok;
        if (c == delim) return ok;
        if (c == ' ' && !ok) continue;
        if (c == '\r') continue;
        if (c < '0' || c > '9') break;
        *data = *data * 10 + c - '0';
        ok = true;
    }
    skipUntil(file, delim);
    return false;
}


void readConfig()
{
    FatFs::FileMode mode = { 0 };
    mode.read = true;
    FRESULT result;
    FatFs::File file("/config.txt", mode, &result);
    if (result != FR_OK) error(3);

    memset(&config, 0, sizeof(config));
    config.interval = 60;
    config.buffer = MAX_BUFFER;
    config.resolution = 12;
    while (!file.eof())
    {
        char tag[10];
        int data;
        int taglen = readUntil(&file, tag, ARRAYLEN(tag), ':');
        if (taglen <= 0) continue;
        if (*tag == '#') continue;
        if (!readInt(&file, &data, '\n')) continue;
        if (taglen == 8 && !memcmp(tag, "interval", 8)) config.interval = MAX(1, data);
        else if (taglen == 6 && !memcmp(tag, "buffer", 6)) config.buffer = MAX(1, MIN(MAX_BUFFER, data));
        else if (taglen == 7 && !memcmp(tag, "sensors", 7)) config.sensors = data;
        else if (taglen == 10 && !memcmp(tag, "resolution", 10)) config.resolution = data;
    }
    if (!config.sensors > MAX_SENSORS) error(4);
}


void safeWrite(FatFs::File* file, const void* data, uint32_t len)
{
    uint32_t written;
    if (file->write(data, len, &written) != FR_OK || len != written) error(7);
}


void safePuts(FatFs::File* file, const char* data)
{
    if (file->puts(data) <= 0) error(7);
}


void safePutc(FatFs::File* file, char c)
{
    if (file->putc(c) <= 0) error(7);
}


void hexdump(FatFs::File* file, const void* data, int len)
{
    uint8_t* ptr = (uint8_t*)data;
    while (len--)
        if (file->printf("%02x", *ptr++) <= 0) error(7);
}


void printNumber(FatFs::File* file, int32_t number)
{
    if (file->printf("%.3D", number) <= 0) error(7);
}


#ifdef HAVE_LCD
int printTempLCD(int row, char label, int32_t number)
{
    return lcd.printf(row, 0, font, 0, "%c:%3.3D°C ", label, number);
}
int printVoltLCD(int row, char label, int32_t number)
{
    return lcd.printf(row, 50, font, 0, "%c:%.3DV ", label, number);
}
#endif


void openLogFile(FatFs::File* file)
{
    FRESULT result = FR_INVALID_OBJECT;
    char filename[] = "/log00.csv";
    FatFs::FileMode mode = { 0 };
    mode.write = true;
    mode.createNew = true;
    for (int i = 0; i <= 100; i++)
    {
        filename[4] = '0' + i / 10;
        filename[5] = '0' + i % 10;
        new(file) FatFs::File(filename, mode, &result);
        if (result == FR_OK) break;
    }

    if (result != FR_OK) error(6);

    safePuts(file,
        "nanologger " VERSION
#ifdef HAVE_LCD
        " with LCD"
#endif
        "\nTime"
    );
    char sensorNum[] = ";Sensor00";
    for (int i = 0; i < config.sensors; i++)
    {
        sensorNum[7] = '0' + i / 10;
        sensorNum[8] = '0' + i % 10;
        safePuts(file, sensorNum);
    }
    safePuts(file, ";ChipTemp;Battery;System;RTCFreq;RTCDev\n");

    hexdump(file, (void*)0x1ffff7ac, 12);
    for (int i = 0; i < config.sensors; i++)
    {
        safePutc(file, ';');
        hexdump(file, sensor[i].getDeviceId(), 8);
    }
    safePuts(file, "\ns");
    for (int i = 0; i < config.sensors; i++) safePuts(file, ";°C");
    safePuts(file, ";°C;V;V;Hz;ppm\n\n");
}


int getTime()
{
    union STM32_RTC_REG_TYPE::TR TR = { STM32_RTC_REGS.TR.d32 };
    int time = ((TR.b.MNT * 10 + TR.b.MNU) * 6 + TR.b.ST) * 10 + TR.b.SU;
    if (time < lastTime) timeOffset += 3600;
    lastTime = time;
    return timeOffset + time;
}


void rtcSleep(int until)
{
    if (until <= getTime()) return;
    int seconds = (until - timeOffset) % 3600;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    union STM32_RTC_REG_TYPE::CR CR = { 0 };
    STM32_RTC_REGS.CR.d32 = CR.d32;
    while (!STM32_RTC_REGS.ISR.b.ALRAWF);
    union STM32_RTC_REG_TYPE::ALRMAR ALRMAR = { 0 };
    ALRMAR.b.MSK4 = true;
    ALRMAR.b.MSK3 = true;
    ALRMAR.b.MNT = minutes / 10;
    ALRMAR.b.MNU = minutes % 10;
    ALRMAR.b.ST = seconds / 10;
    ALRMAR.b.SU = seconds % 10;
    STM32_RTC_REGS.ALRMAR.d32 = ALRMAR.d32;
    CR.b.ALRAE = true;
    CR.b.ALRAIE = true;
    STM32_RTC_REGS.CR.d32 = CR.d32;
    enter_critical_section();
    while (until > getTime())
    {
#ifndef DEBUG
        SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
#endif
        idle();
        SCB->SCR = 0;
    }
    leave_critical_section();
}


void readVoltages(struct DataPoint* data)
{
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
    STM32_ADC1_REGS.CHSELR.d32 = 1 << BATT_ADC_INPUT;
    CR.b.ADSTART = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    uint32_t system = (3300 << 8) * *((uint16_t*)0x1ffff7ba) / adRef;
    data->systemVoltage = system >> 8;
    while (!STM32_ADC1_REGS.ISR.b.EOC);
    uint16_t adBatt = STM32_ADC1_REGS.DR;
    STM32_ADC1_REGS.SMPR = 7;
    STM32_ADC1_REGS.CHSELR.d32 = 1 << 16;
    CR.b.ADSTART = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    data->batteryVoltage = (adBatt * system) >> 20;
    while (!STM32_ADC1_REGS.ISR.b.EOC);
    uint16_t adTemp = STM32_ADC1_REGS.DR;
    CR.d32 = 0;
    CR.b.ADDIS = true;
    STM32_ADC1_REGS.CR.d32 = CR.d32;
    STM32_ADC_REGS.CCR.d32 = 0;
    int temp = ((int)((adTemp << 7) * *((uint16_t*)0x1ffff7ba) / adRef)) - ((int)(*((uint16_t*)0x1ffff7b8) << 7));
    data->chipTemp = ((temp * 8000) >> 7) / (*((uint16_t*)0x1ffff7c2) - *((uint16_t*)0x1ffff7b8)) + 3000;
    while (STM32_ADC1_REGS.CR.b.ADEN);
    clockgate_enable(STM32_ADC_CLOCKGATE, true);
}


int getRTCFrequency()
{
    clockgate_enable(STM32_TIM14_CLOCKGATE, true);
    union STM32_TIM_REG_TYPE::OR OR = { 0 };
    OR.b.TI1_RMP = 1;
    STM32_TIM14_REGS.OR.d32 = OR.d32;
    union STM32_TIM_REG_TYPE::CCMR1 CCMR1 = { 0 };
    CCMR1.ic.CC1S = 1;
    CCMR1.ic.IC1PSC = 3;
    STM32_TIM14_REGS.CCMR1.d32 = CCMR1.d32;
    union STM32_TIM_REG_TYPE::CCER CCER = { 0 };
    CCER.b.CC1E = true;
    STM32_TIM14_REGS.CCER.d32 = CCER.d32;
    union STM32_TIM_REG_TYPE::CR1 CR1 = { 0 };
    STM32_TIM14_REGS.CR1.d32 = CR1.d32;
    STM32_TIM14_REGS.PSC = 0;
    STM32_TIM14_REGS.ARR = 0xffff;
    union STM32_TIM_REG_TYPE::EGR EGR = { 0 };
    EGR.b.UG = true;
    STM32_TIM14_REGS.EGR.d32 = EGR.d32;
    union STM32_TIM_REG_TYPE::SR SR = { 0 };
    SR.b.CC1IF = true;
    STM32_TIM14_REGS.SR.d32 = SR.d32;
    CR1.b.CEN = true;
    STM32_TIM14_REGS.CR1.d32 = CR1.d32;
    while (!STM32_TIM14_REGS.SR.b.CC1IF);
    int begin = STM32_TIM14_REGS.CCR1;
    STM32_TIM14_REGS.SR.d32 = SR.d32;
    while (!STM32_TIM14_REGS.SR.b.CC1IF);
    int end = STM32_TIM14_REGS.CCR1;
    STM32_TIM14_REGS.SR.d32 = SR.d32;
    STM32_TIM14_REGS.CR1.d32 = 0;
    clockgate_enable(STM32_TIM14_CLOCKGATE, false);
    return STM32_AHB_CLOCK * 8 / (end - begin);
}


void calibrateRTC()
{
    int actualFreq = getRTCFrequency();
    rtcDeviation = MAX(-32768, MIN(32767, (MAX(-2048, MIN(2047, actualFreq - rtcFrequency)) << 20) / actualFreq));
    int deviation = MAX(-511, MIN(512, rtcDeviation));
    union STM32_RTC_REG_TYPE::CALR CALR = { 0 };
    CALR.b.CALP = deviation < 0;
    CALR.b.CALM = deviation;
    STM32_RTC_REGS.CALR.d32 = CALR.d32;
}


void initRTC()
{
    while (!STM32_RCC_REGS.CSR.b.LSIRDY);
    union STM32_RCC_REG_TYPE::BDCR BDCR = { 0 };
    BDCR.b.BDRST = true;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
    BDCR.d32 = 0;
    BDCR.b.RTCEN = true;
    BDCR.b.RTCSEL = BDCR.b.RTCSEL_LSI;
    STM32_RCC_REGS.BDCR.d32 = BDCR.d32;
    STM32_RTC_REGS.WPR = 0xca;
    STM32_RTC_REGS.WPR = 0x53;
    union STM32_RTC_REG_TYPE::ISR ISR = { 0 };
    ISR.b.INIT = true;
    STM32_RTC_REGS.ISR.d32 = ISR.d32;
    while (!STM32_RTC_REGS.ISR.b.INITF);
    union STM32_RTC_REG_TYPE::PRER PRER = { 0 };
    rtcFrequency = (getRTCFrequency() >> 2) << 2;
    PRER.b.PREDIV_A = (1 << 2) - 1;
    PRER.b.PREDIV_S = (rtcFrequency >> 2) - 1;
    STM32_RTC_REGS.PRER.d32 = PRER.d32;
    STM32_RTC_REGS.DR.d32 = 0;
    STM32_RTC_REGS.TR.d32 = 0;
    STM32_RTC_REGS.ISR.d32 = 0;
    STM32_EXTI_REGS.RTSR = 1 << 17;
    STM32_EXTI_REGS.IMR = 1 << 17;
    irq_enable(rtc_IRQn, true);
}


extern "C" void rtc_irqhandler()
{
    STM32_RTC_REGS.CR.d32 = 0;
    STM32_RTC_REGS.ISR.d32 = 0;
    STM32_EXTI_REGS.PR = 1 << 17;
}


void __attribute__((noreturn)) startLogging()
{
    FatFs::File file;
    openLogFile(&file);

    readVoltages(dataBuffer);
    int level = MAX(1, (dataBuffer->batteryVoltage - 2000) / 200);
    for (int i = 0; i < level; i++)
    {
        GPIO::setLevel(PIN_LED, true);
        udelay(50000);
        GPIO::setLevel(PIN_LED, false);
        udelay(300000);
    }

    initRTC();

#ifdef HAVE_LCD
    lcd.clear();
    lcd.printf(2, 50, font, 0, "R%6d", rtcFrequency);
#endif

    int next = getTime();
    int ptr = 0;
    int total = 0;
    while (true)
    {
        int now;
        while (!TIME_AFTER(now = getTime(), next)) rtcSleep(next + 1);
        next = next + config.interval;
        dataBuffer[ptr].time = now;
        readVoltages(dataBuffer + ptr);
        bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(PIN_SENSOR.pin >> 4), true);
        for (int i = 0; i < config.sensors; i++)
            dataBuffer[ptr].sensor[i] = sensor[i].readRawTemperature();
        clockgate_enable(STM32_GPIO_CLOCKGATE(PIN_SENSOR.pin >> 4), old);
#ifdef HAVE_LCD
        printTempLCD(0, 'C', dataBuffer[ptr].chipTemp * 10);
        for (int i = 0; i < MIN(5, config.sensors); i++)
            printTempLCD(i + 1, '0' + i, sensor[i].scaleRawTemperature(dataBuffer[ptr].sensor[i]));
        printVoltLCD(0, 'B', dataBuffer[ptr].batteryVoltage);
        printVoltLCD(1, 'S', dataBuffer[ptr].systemVoltage);
        lcd.printf(4, 50, font, 0, "%7d", total);
        lcd.printf(5, 50, font, 0, "%3d/%3d", ptr, config.buffer);
#endif
        total++;
        ptr++;
        if (ptr >= config.buffer)
        {
            GPIO::setLevel(PIN_LED, true);
            calibrateRTC();
            for (int i = 0; i < ptr; i++)
            {
                if (file.printf("%d", dataBuffer[i].time) <= 0) error(7);
                for (int j = 0; j < config.sensors; j++)
                {
                    safePutc(&file, ';');
                    printNumber(&file, sensor[j].scaleRawTemperature(dataBuffer[i].sensor[j]));
                }
                safePutc(&file, ';');
                printNumber(&file, dataBuffer[i].chipTemp * 10);
                safePutc(&file, ';');
                printNumber(&file, dataBuffer[i].batteryVoltage);
                safePutc(&file, ';');
                printNumber(&file, dataBuffer[i].systemVoltage);
                if (i == ptr - 1)
                {
                    if (file.printf(";%d;%d", rtcFrequency, rtcDeviation) <= 0) error(7);
                }
                safePutc(&file, '\n');
            }
            file.sync();
            ptr = 0;
#ifdef HAVE_LCD
            lcd.printf(3, 50, font, 0, "D%6d", rtcDeviation);
#endif
            GPIO::setLevel(PIN_LED, false);
        }
    }
}


void initSensors()
{
    bool old = clockgate_enable(STM32_GPIO_CLOCKGATE(PIN_SENSOR.pin >> 4), true);

    for (int i = 0; i < config.sensors; i++)
    {
        uint64_t* deviceId = onewire.discoverDevice();
        if (!deviceId) error(5);
        new(&sensor[i]) DS1820(&onewire, *deviceId);
        sensor[i].setResolution(config.resolution);
    }

    for (int i = 0; i < config.sensors; i++) sensor[i].readRawTemperature();
    clockgate_enable(STM32_GPIO_CLOCKGATE(PIN_SENSOR.pin >> 4), old);
}


int main()
{
#ifdef HAVE_LCD
    lcd.printf(0, 0, font, 0, "nanologger " VERSION " by");
    lcd.printf(1, 0, font, 0, "Michael Sparmann");
#endif

    clockgate_enable(STM32_PWR_CLOCKGATE, true);
    union STM32_PWR_REG_TYPE::CR CR = { 0 };
    CR.b.DBP = true;
    CR.b.LPDS = true;
    STM32_PWR_REGS.CR.d32 = CR.d32;
    clockgate_enable(STM32_PWR_CLOCKGATE, false);
    union STM32_RCC_REG_TYPE::CSR CSR = { 0 };
    CSR.b.LSION = true;
    STM32_RCC_REGS.CSR.d32 = CSR.d32;

    if (sd.getStatus() != Storage::RESULT_OK) error(1);
    if (mountResult != FR_OK) error(2);

    readConfig();

    initSensors();

    startLogging();
}
