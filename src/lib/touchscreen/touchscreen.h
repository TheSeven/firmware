#pragma once

#include "global.h"
#include "interface/display/addressabledisplay.h"
#include "interface/touchpanel/touchpanel.h"


class __attribute__((packed,aligned(4))) TouchScreen
{
public:
    struct CalibData
    {
        int16_t xOffset;
        int16_t xScale;
        int16_t yOffset;
        int16_t yScale;
    };

    CalibData calib;
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
    TouchScreen(Display::AddressableDisplay* display, TouchPanel* touchPanel);
    void calibrate();
    bool read();

private:
    Display::AddressableDisplay* display;
    TouchPanel* panel;
    void getPoint(int offset, uint16_t* x, uint16_t* y);
};
