#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"


#define STM32_GPIO_PORTA (0 << 4)
#define STM32_GPIO_PORTB (1 << 4)
#define STM32_GPIO_PORTC (2 << 4)
#define STM32_GPIO_PORTD (3 << 4)
#define STM32_GPIO_PORTE (4 << 4)
#define STM32_GPIO_PORTF (5 << 4)
#define STM32_GPIO_PORTG (6 << 4)

#define PIN_A0 (::GPIO::Pin(0, STM32_GPIO_PORTA|0))
#define PIN_A1 (::GPIO::Pin(0, STM32_GPIO_PORTA|1))
#define PIN_A2 (::GPIO::Pin(0, STM32_GPIO_PORTA|2))
#define PIN_A3 (::GPIO::Pin(0, STM32_GPIO_PORTA|3))
#define PIN_A4 (::GPIO::Pin(0, STM32_GPIO_PORTA|4))
#define PIN_A5 (::GPIO::Pin(0, STM32_GPIO_PORTA|5))
#define PIN_A6 (::GPIO::Pin(0, STM32_GPIO_PORTA|6))
#define PIN_A7 (::GPIO::Pin(0, STM32_GPIO_PORTA|7))
#define PIN_A8 (::GPIO::Pin(0, STM32_GPIO_PORTA|8))
#define PIN_A9 (::GPIO::Pin(0, STM32_GPIO_PORTA|9))
#define PIN_A10 (::GPIO::Pin(0, STM32_GPIO_PORTA|10))
#define PIN_A11 (::GPIO::Pin(0, STM32_GPIO_PORTA|11))
#define PIN_A12 (::GPIO::Pin(0, STM32_GPIO_PORTA|12))
#define PIN_A13 (::GPIO::Pin(0, STM32_GPIO_PORTA|13))
#define PIN_A14 (::GPIO::Pin(0, STM32_GPIO_PORTA|14))
#define PIN_A15 (::GPIO::Pin(0, STM32_GPIO_PORTA|15))
#define PIN_B0 (::GPIO::Pin(0, STM32_GPIO_PORTB|0))
#define PIN_B1 (::GPIO::Pin(0, STM32_GPIO_PORTB|1))
#define PIN_B2 (::GPIO::Pin(0, STM32_GPIO_PORTB|2))
#define PIN_B3 (::GPIO::Pin(0, STM32_GPIO_PORTB|3))
#define PIN_B4 (::GPIO::Pin(0, STM32_GPIO_PORTB|4))
#define PIN_B5 (::GPIO::Pin(0, STM32_GPIO_PORTB|5))
#define PIN_B6 (::GPIO::Pin(0, STM32_GPIO_PORTB|6))
#define PIN_B7 (::GPIO::Pin(0, STM32_GPIO_PORTB|7))
#define PIN_B8 (::GPIO::Pin(0, STM32_GPIO_PORTB|8))
#define PIN_B9 (::GPIO::Pin(0, STM32_GPIO_PORTB|9))
#define PIN_B10 (::GPIO::Pin(0, STM32_GPIO_PORTB|10))
#define PIN_B11 (::GPIO::Pin(0, STM32_GPIO_PORTB|11))
#define PIN_B12 (::GPIO::Pin(0, STM32_GPIO_PORTB|12))
#define PIN_B13 (::GPIO::Pin(0, STM32_GPIO_PORTB|13))
#define PIN_B14 (::GPIO::Pin(0, STM32_GPIO_PORTB|14))
#define PIN_B15 (::GPIO::Pin(0, STM32_GPIO_PORTB|15))
#define PIN_C0 (::GPIO::Pin(0, STM32_GPIO_PORTC|0))
#define PIN_C1 (::GPIO::Pin(0, STM32_GPIO_PORTC|1))
#define PIN_C2 (::GPIO::Pin(0, STM32_GPIO_PORTC|2))
#define PIN_C3 (::GPIO::Pin(0, STM32_GPIO_PORTC|3))
#define PIN_C4 (::GPIO::Pin(0, STM32_GPIO_PORTC|4))
#define PIN_C5 (::GPIO::Pin(0, STM32_GPIO_PORTC|5))
#define PIN_C6 (::GPIO::Pin(0, STM32_GPIO_PORTC|6))
#define PIN_C7 (::GPIO::Pin(0, STM32_GPIO_PORTC|7))
#define PIN_C8 (::GPIO::Pin(0, STM32_GPIO_PORTC|8))
#define PIN_C9 (::GPIO::Pin(0, STM32_GPIO_PORTC|9))
#define PIN_C10 (::GPIO::Pin(0, STM32_GPIO_PORTC|10))
#define PIN_C11 (::GPIO::Pin(0, STM32_GPIO_PORTC|11))
#define PIN_C12 (::GPIO::Pin(0, STM32_GPIO_PORTC|12))
#define PIN_C13 (::GPIO::Pin(0, STM32_GPIO_PORTC|13))
#define PIN_C14 (::GPIO::Pin(0, STM32_GPIO_PORTC|14))
#define PIN_C15 (::GPIO::Pin(0, STM32_GPIO_PORTC|15))
#define PIN_D0 (::GPIO::Pin(0, STM32_GPIO_PORTD|0))
#define PIN_D1 (::GPIO::Pin(0, STM32_GPIO_PORTD|1))
#define PIN_D2 (::GPIO::Pin(0, STM32_GPIO_PORTD|2))
#define PIN_D3 (::GPIO::Pin(0, STM32_GPIO_PORTD|3))
#define PIN_D4 (::GPIO::Pin(0, STM32_GPIO_PORTD|4))
#define PIN_D5 (::GPIO::Pin(0, STM32_GPIO_PORTD|5))
#define PIN_D6 (::GPIO::Pin(0, STM32_GPIO_PORTD|6))
#define PIN_D7 (::GPIO::Pin(0, STM32_GPIO_PORTD|7))
#define PIN_D8 (::GPIO::Pin(0, STM32_GPIO_PORTD|8))
#define PIN_D9 (::GPIO::Pin(0, STM32_GPIO_PORTD|9))
#define PIN_D10 (::GPIO::Pin(0, STM32_GPIO_PORTD|10))
#define PIN_D11 (::GPIO::Pin(0, STM32_GPIO_PORTD|11))
#define PIN_D12 (::GPIO::Pin(0, STM32_GPIO_PORTD|12))
#define PIN_D13 (::GPIO::Pin(0, STM32_GPIO_PORTD|13))
#define PIN_D14 (::GPIO::Pin(0, STM32_GPIO_PORTD|14))
#define PIN_D15 (::GPIO::Pin(0, STM32_GPIO_PORTD|15))
#define PIN_E0 (::GPIO::Pin(0, STM32_GPIO_PORTE|0))
#define PIN_E1 (::GPIO::Pin(0, STM32_GPIO_PORTE|1))
#define PIN_E2 (::GPIO::Pin(0, STM32_GPIO_PORTE|2))
#define PIN_E3 (::GPIO::Pin(0, STM32_GPIO_PORTE|3))
#define PIN_E4 (::GPIO::Pin(0, STM32_GPIO_PORTE|4))
#define PIN_E5 (::GPIO::Pin(0, STM32_GPIO_PORTE|5))
#define PIN_E6 (::GPIO::Pin(0, STM32_GPIO_PORTE|6))
#define PIN_E7 (::GPIO::Pin(0, STM32_GPIO_PORTE|7))
#define PIN_E8 (::GPIO::Pin(0, STM32_GPIO_PORTE|8))
#define PIN_E9 (::GPIO::Pin(0, STM32_GPIO_PORTE|9))
#define PIN_E10 (::GPIO::Pin(0, STM32_GPIO_PORTE|10))
#define PIN_E11 (::GPIO::Pin(0, STM32_GPIO_PORTE|11))
#define PIN_E12 (::GPIO::Pin(0, STM32_GPIO_PORTE|12))
#define PIN_E13 (::GPIO::Pin(0, STM32_GPIO_PORTE|13))
#define PIN_E14 (::GPIO::Pin(0, STM32_GPIO_PORTE|14))
#define PIN_E15 (::GPIO::Pin(0, STM32_GPIO_PORTE|15))
#define PIN_F0 (::GPIO::Pin(0, STM32_GPIO_PORTF|0))
#define PIN_F1 (::GPIO::Pin(0, STM32_GPIO_PORTF|1))
#define PIN_F2 (::GPIO::Pin(0, STM32_GPIO_PORTF|2))
#define PIN_F3 (::GPIO::Pin(0, STM32_GPIO_PORTF|3))
#define PIN_F4 (::GPIO::Pin(0, STM32_GPIO_PORTF|4))
#define PIN_F5 (::GPIO::Pin(0, STM32_GPIO_PORTF|5))
#define PIN_F6 (::GPIO::Pin(0, STM32_GPIO_PORTF|6))
#define PIN_F7 (::GPIO::Pin(0, STM32_GPIO_PORTF|7))
#define PIN_F8 (::GPIO::Pin(0, STM32_GPIO_PORTF|8))
#define PIN_F9 (::GPIO::Pin(0, STM32_GPIO_PORTF|9))
#define PIN_F10 (::GPIO::Pin(0, STM32_GPIO_PORTF|10))
#define PIN_F11 (::GPIO::Pin(0, STM32_GPIO_PORTF|11))
#define PIN_F12 (::GPIO::Pin(0, STM32_GPIO_PORTF|12))
#define PIN_F13 (::GPIO::Pin(0, STM32_GPIO_PORTF|13))
#define PIN_F14 (::GPIO::Pin(0, STM32_GPIO_PORTF|14))
#define PIN_F15 (::GPIO::Pin(0, STM32_GPIO_PORTF|15))
#define PIN_G0 (::GPIO::Pin(0, STM32_GPIO_PORTG|0))
#define PIN_G1 (::GPIO::Pin(0, STM32_GPIO_PORTG|1))
#define PIN_G2 (::GPIO::Pin(0, STM32_GPIO_PORTG|2))
#define PIN_G3 (::GPIO::Pin(0, STM32_GPIO_PORTG|3))
#define PIN_G4 (::GPIO::Pin(0, STM32_GPIO_PORTG|4))
#define PIN_G5 (::GPIO::Pin(0, STM32_GPIO_PORTG|5))
#define PIN_G6 (::GPIO::Pin(0, STM32_GPIO_PORTG|6))
#define PIN_G7 (::GPIO::Pin(0, STM32_GPIO_PORTG|7))
#define PIN_G8 (::GPIO::Pin(0, STM32_GPIO_PORTG|8))
#define PIN_G9 (::GPIO::Pin(0, STM32_GPIO_PORTG|9))
#define PIN_G10 (::GPIO::Pin(0, STM32_GPIO_PORTG|10))
#define PIN_G11 (::GPIO::Pin(0, STM32_GPIO_PORTG|11))
#define PIN_G12 (::GPIO::Pin(0, STM32_GPIO_PORTG|12))
#define PIN_G13 (::GPIO::Pin(0, STM32_GPIO_PORTG|13))
#define PIN_G14 (::GPIO::Pin(0, STM32_GPIO_PORTG|14))
#define PIN_G15 (::GPIO::Pin(0, STM32_GPIO_PORTG|15))


namespace STM32
{

    class __attribute__((packed,aligned(4))) GPIO_F1 final
    {
    public:
        enum Speed
        {
            SPEED_INPUT = 0,
            SPEED_10MHZ = 1,
            SPEED_2MHZ = 2,
            SPEED_50MHZ = 3,
        };

        class PinController final : ::GPIO::PinController
        {
        public:
            virtual bool getLevel(unsigned int pin) const;
            virtual void setLevel(unsigned int pin, bool level) const;
            virtual void setMode(unsigned int pin, enum ::GPIO::mode mode) const;
            virtual void setType(unsigned int pin, enum ::GPIO::type type) const;
            virtual void setPull(unsigned int pin, enum ::GPIO::pull pull) const;
            virtual void setSpecial(unsigned int pin, int function) const;

            void setSpeed(unsigned int pin, enum Speed speed) const;
        };
        static const PinController Controller;

        static void init();

    };

}

