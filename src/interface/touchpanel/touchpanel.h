#pragma once

#include "global.h"


class __attribute__((packed,aligned(4))) TouchPanel
{
public:
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
    virtual bool update() = 0;
};
