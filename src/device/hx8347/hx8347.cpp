#include "global.h"
#include "device/hx8347/hx8347.h"
#include "sys/time.h"
#include "sys/util.h"


static const uint8_t hx8347_initscript[] =
{
  0x23, 0x8d,  // VCOM control 1 (VCOM offset)
  0x24, 0x2f,  // VCOM control 2 (VCOMH)
  0x25, 0x57,  // VCOM control 3 (VCOML)
  0x18, 0x36,  // OSC control 1 (frame rate)
  0x19, 0x01,  // OSC control 2 (OSC enable)
  0x01, 0x00,  // Display mode control (standby, idle, invert, partial, scroll)
  0x1f, 0x88,  // Power control 6 (standby, charge pump control)
  0x1f, 0x80,  // Power control 6 (standby, charge pump control)
  0x1f, 0x90,  // Power control 6 (standby, charge pump control)
  0x1f, 0xd0,  // Power control 6 (standby, charge pump control)
  0x17, 0x05,  // COLMOD (bits per pixel)
  0x28, 0x38,  // Display control 3 (display enable)
  0
};


HX8347::HX8347(const SPI::Bus* bus, GPIO::Pin cspin, int frequency)
    : Device(bus, cspin, frequency), AddressableDisplay(320, 240, 2, ::Display::ColorFormat::RGB565),
      inDataMode(false), currentMode({false, false, false, false}), direction({false, false, false})
{
    select();
    init();
    deselect();
}

void HX8347::select()
{
    Device::select();
}

void HX8347::deselect()
{
    Device::deselect();
}

void HX8347::selectReg(uint8_t reg, bool read)
{
    inDataMode = false;
    reselect();
    pushByte(0x70);
    pushByte(reg);
    reselect();
    pushByte(0x72 | read);
}

void HX8347::writeReg(uint8_t reg, uint8_t data)
{
    selectReg(reg, false);
    pushByte(data);
}

uint8_t HX8347::readReg(uint8_t reg)
{
    selectReg(reg, true);
    return pushByte(0xff);
}

void HX8347::enterDataMode()
{
    selectReg(0x22, false);
    inDataMode = true;
}

void HX8347::powerUp()
{
    const uint8_t* script = hx8347_initscript;
    while (uint8_t reg = *script++) writeReg(reg, *script++);
}

void HX8347::init()
{
    powerUp();
    writeReg(0x16, 0x08);
    clear(0);
    writeReg(0x28, 0x3c);
}

void HX8347::setMode(::Display::DisplayMode mode)
{
    if (mode.sleep)
    {
        if (currentMode.sleep) return;
        writeReg(0x28, 0x38);
        udelay(40000);
        writeReg(0x28, 0x04);
        writeReg(0x1f, 0x89);
        writeReg(0x19, 0x00);
    }
    if (currentMode.sleep)
    {
        powerUp();
        if (!mode.blank) udelay(40000);
    }
    writeReg(0x28, 0x38 | (!mode.blank << 2));
    writeReg(0x01, (mode.lowPowerMode << 2) | (mode.invert << 1));
    currentMode = mode;
}

void HX8347::setBrightness(int brightness)
{
    //TODO: Implement
}

void HX8347::setContrast(int contrast)
{
    //TODO: Implement
}

void HX8347::setDirection(Direction direction)
{
    this->direction = direction;
    uint8_t byte = 0x08;
    if (direction.hInvert) byte |= 0x80;
    if (direction.vInvert) byte |= 0x40;
    if (direction.vertical) byte |= 0x20;
    writeReg(0x16, byte);
}

void HX8347::setRange(int row, int col, int height, int width)
{
    if (direction.hInvert) col = 319 - col;
    if (direction.vInvert) row = 239 - row;
    if (direction.vertical)
    {
        int temp = row;
        row = col;
        col = temp;
        temp = height;
        height = width;
        width = temp;
    }
    writeReg(0x02, col >> 8);  // Column address start high
    writeReg(0x03, col);  // Column address start low
    writeReg(0x04, (col + width - 1) >> 8);  // Column address end high
    writeReg(0x05, col + width - 1);  // Column address end low
    writeReg(0x06, row >> 8);  // Row address start high
    writeReg(0x07, row);  // Row address start low
    writeReg(0x08, (row + height - 1) >> 8);  // Row address end high
    writeReg(0x09, row + height - 1);  // Row address end low
}

void HX8347::writeData(void* data, int pixels)
{
    if (!inDataMode) enterDataMode();
    pushBuffer(data, NULL, pixels * bytesPerPixel);
}

void HX8347::writeColor(uint32_t color, int pixels)
{
    if (!inDataMode) enterDataMode();
    while (pixels--)
    {
        pushByte(color & 0xff);
        pushByte(color >> 8);
    }
}
