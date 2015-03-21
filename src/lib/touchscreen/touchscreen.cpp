#include "global.h"
#include "lib/touchscreen/touchscreen.h"
#include "sys/util.h"


TouchScreen::TouchScreen(Display::AddressableDisplay* display, TouchPanel* touchPanel)
    : display(display), panel(touchPanel)
{
    read();
}

void TouchScreen::getPoint(int offset, uint16_t* x, uint16_t* y)
{
    int xunit = display->width >> 4;
    int yunit = display->height >> 4;
    display->select();
    display->clear(0);
    display->fill(yunit * offset, xunit * (offset + 1), yunit * 2, 1, 0xffffffff);
    display->fill(yunit * (offset + 1), xunit * offset, 1, xunit * 2, 0xffffffff);
    display->deselect();
    while (!panel->update());
    do
    {
        *x = panel->x;
        *y = panel->y;
    }
    while (panel->update());
}

void TouchScreen::calibrate()
{
    uint16_t lx;
    uint16_t ly;
    uint16_t hx;
    uint16_t hy;
    getPoint(1, &lx, &ly);
    getPoint(13, &hx, &hy);
    display->select();
    display->clear(0);
    display->deselect();
    calib.xScale = (12 << 24) / (hx - lx);
    calib.xOffset = lx - ((2 << 24) / calib.xScale);
    calib.yScale = (12 << 24) / (hy - ly);
    calib.yOffset = ly - ((2 << 24) / calib.yScale);
}

bool TouchScreen::read()
{
    bool pressed = panel->update();
    pressure = panel->pressure;
    x = MAX(0, MIN(display->width, ((((panel->x - calib.xOffset) * calib.xScale) >> 12) * display->width) >> 16));
    y = MAX(0, MIN(display->height, ((((panel->y - calib.yOffset) * calib.yScale) >> 12) * display->height) >> 16));
    return pressed;
}
