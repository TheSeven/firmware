#pragma once

#include "global.h"


namespace TM1829
{

    class __attribute__((packed,aligned(4))) Strip
    {
    public:
        virtual void sendFrame(void** data, int pixels) = 0;
        virtual bool isSending() = 0;
        virtual void cancelSending() = 0;
    };

}
