#pragma once

#include "global.h"


#ifndef GPIO_OPTIMIZE
#define GPIO_OPTIMIZE __attribute__((optimize("-Os")))
#endif


class __attribute__((packed,aligned(4))) GPIO
{
public:
    enum mode
    {
        MODE_INPUT = 0,
        MODE_OUTPUT = 1,
        MODE_SPECIAL = 2,
        MODE_ANALOG = 3,
    };

    enum type
    {
        TYPE_PUSHPULL = 0,
        TYPE_OPENDRAIN = 1,
    };

    enum pull
    {
        PULL_NONE = 0,
        PULL_UP = 1,
        PULL_DOWN = 2,
    };
    
    struct __attribute__((packed)) Pin
    {
        unsigned int pin : 8;
#ifdef GPIO_DYNAMIC_CONTROLLERS
        unsigned int controller : 8;
#endif
        constexpr Pin() : pin(0)
#ifdef GPIO_DYNAMIC_CONTROLLERS
            , controller(0)
#endif
        {}
        constexpr Pin(unsigned int controller, unsigned int pin) : pin(pin)
#ifdef GPIO_DYNAMIC_CONTROLLERS
            , controller(controller)
#endif
        {}
    };

    class PinController
    {
        virtual bool getLevel(unsigned int pin) const = 0;
        virtual void setLevel(unsigned int pin, bool level) const = 0;
        virtual void setMode(unsigned int pin, enum mode mode) const = 0;
        virtual void setType(unsigned int pin, enum type type) const = 0;
        virtual void setPull(unsigned int pin, enum pull pull) const = 0;
        virtual void setSpecial(unsigned int pin, int function) const = 0;
#ifdef GPIO_SUPPORT_FAST_MODE
        virtual bool enableFast(unsigned int pin, bool on) const = 0;
        virtual bool getLevelFast(unsigned int pin) const = 0;
        virtual void setLevelFast(unsigned int pin, bool level) const = 0;
#endif
    };

    static bool getLevel(Pin pin);
    static void setLevel(Pin pin, bool level);
    static void setMode(Pin pin, enum mode mode);
    static void setType(Pin pin, enum type type);
    static void setPull(Pin pin, enum pull pull);
    static void setSpecial(Pin pin, int function);
    static void configure(Pin pin, enum mode mode, bool level);
    static void configure(Pin pin, int function);
    static void configure(Pin pin, enum mode mode, bool level, enum type type, enum pull pull, int function);
    static bool enableFast(Pin pin, bool on);
    static bool getLevelFast(Pin pin);
    static void setLevelFast(Pin pin, bool level);

    static inline void configure(Pin pin, int function, enum type type, enum pull pull)
    {
        configure(pin, MODE_SPECIAL, false, type, pull, function);
    }

    static inline void configure(Pin pin, enum mode mode, bool level, enum type type, enum pull pull)
    {
        configure(pin, mode, level, type, pull, 0);
    }

    static inline void configure(Pin pin, enum mode mode)
    {
        configure(pin, mode, false);
    }
};
