#include "global.h"
#include "device/st7735/st7735.h"
#include "sys/time.h"
#include "sys/util.h"


static const uint8_t st7735_initscript[] =
{
    0x01, 0,  // Software reset
    0x11, 0,  // Sleep out
    0x3a, 1, 0x05,  // Interface pixel format: RGB565
    0x36, 1, 0x08,  // Memory data access control: Normal access. Colors: BRG, Refresh: left to right, top to bottom
    0xb4, 1, 0x00,  // Display inversion control: Always dot inversion
    0xc0, 3, 0xa2, 0x02, 0x84,  // Power control 1: AVDD=5.0V, GVDD=4.60V, GVCL=-4.60V, Mode: Auto
    //?0xc4, 2, 0x8a, 0xee,  // Power control (partial mode): AP medium low, SAP small, Boost cycle: BCLK/2,4,2,4,2
    0xc5, 1, 0x0e,  // VCOM control 1: VCOM=-0.775V
    // Gamma correction (positive, negative):
    //0xe0, 16, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
    //0xe1, 16, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
    0
};


ST7735::ST7735(const SPI::Bus* bus, GPIO::Pin csPin, int frequency, GPIO::Pin cdPin)
    : Device(bus, csPin, frequency), AddressableDisplay(160, 128, 2, ::Display::ColorFormat::RGB565),
      cdPin(cdPin), inDataMode(false), currentMode({false, false, false, false}), direction({false, false, false})
{
    GPIO::setMode(cdPin, GPIO::MODE_OUTPUT);
    select();
    init();
    deselect();
}

void ST7735::select()
{
    Device::select();
}

void ST7735::deselect()
{
    Device::deselect();
}

void ST7735::cmd(uint8_t cmd)
{
    GPIO::setLevel(cdPin, false);
    pushByte(cmd);
    GPIO::setLevel(cdPin, true);
    inDataMode = cmd == 0x2c;
}

void ST7735::powerUp()
{
    const uint8_t* script = st7735_initscript;
    while (uint8_t reg = *script++)
    {
        cmd(reg);
        uint8_t len = *script++;
        if (len) pushBuffer(script, NULL, len);
        script += len;
    }
}

void ST7735::init()
{
    powerUp();
    clear(0);
    cmd(0x29);
    udelay(20000);
}

void ST7735::setMode(::Display::DisplayMode mode)
{
    if (mode.sleep ^ currentMode.sleep)
    {
        cmd(mode.sleep ? 0x10 : 0x11);
        if (!mode.sleep && !mode.blank) udelay(25000);
    }
    if (mode.lowPowerMode ^ currentMode.lowPowerMode) cmd(mode.lowPowerMode ? 0x39 : 0x38);
    if (mode.blank ^ currentMode.blank) cmd(mode.blank ? 0x28 : 0x29);
    if (mode.invert ^ currentMode.invert) cmd(mode.invert ? 0x20 : 0x21);
    currentMode = mode;
}

void ST7735::setBrightness(int brightness)
{
    //TODO: Implement
}

void ST7735::setContrast(int contrast)
{
    //TODO: Implement
}

void ST7735::setDirection(Direction direction)
{
    this->direction = direction;
    uint8_t byte = 0x08;
    if (direction.hInvert) byte |= 0x80;
    if (direction.vInvert) byte |= 0x40;
    if (direction.vertical) byte |= 0x20;
    cmd(0x36);
    pushByte(byte);
}

void ST7735::setRange(int row, int col, int height, int width)
{
    if (direction.vertical)
    {
        int temp = row;
        row = col;
        col = temp;
        temp = height;
        height = width;
        width = temp;
    }
    cmd(0x2a);
    pushByte(col >> 8);  // Column address start high
    pushByte(col);  // Column address start low
    pushByte((col + width - 1) >> 8);  // Column address end high
    pushByte(col + width - 1);  // Column address end low
    cmd(0x2b);
    pushByte(row >> 8);  // Row address start high
    pushByte(row);  // Row address start low
    pushByte((row + height - 1) >> 8);  // Row address end high
    pushByte(row + height - 1);  // Row address end low
}

void ST7735::writeData(void* data, int pixels)
{
    if (!inDataMode) cmd(0x2c);
    pushBuffer(data, NULL, pixels * bytesPerPixel);
}

void ST7735::writeColor(uint32_t color, int pixels)
{
    if (!inDataMode) cmd(0x2c);
    while (pixels--)
    {
        pushByte(color & 0xff);
        pushByte(color >> 8);
    }
}

bool ST7735::stayAwake(bool on)
{
    return SPI::Device::stayAwake(on);
}
