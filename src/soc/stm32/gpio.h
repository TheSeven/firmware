#pragma once

#include "global.h"
#include "interface/gpio/gpio.h"


#ifndef STM32_GPIO_OPTIMIZE
#define STM32_GPIO_OPTIMIZE
#endif


#define STM32_GPIO_PORTA (0 << 4)
#define STM32_GPIO_PORTB (1 << 4)
#define STM32_GPIO_PORTC (2 << 4)
#define STM32_GPIO_PORTD (3 << 4)
#define STM32_GPIO_PORTE (4 << 4)
#define STM32_GPIO_PORTF (5 << 4)
#define STM32_GPIO_PORTG (6 << 4)
#define STM32_GPIO_PORTH (7 << 4)
#define STM32_GPIO_PORTI (8 << 4)
#define STM32_GPIO_PORTJ (9 << 4)
#define STM32_GPIO_PORTK (10 << 4)

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
#define PIN_H0 (::GPIO::Pin(0, STM32_GPIO_PORTH|0))
#define PIN_H1 (::GPIO::Pin(0, STM32_GPIO_PORTH|1))
#define PIN_H2 (::GPIO::Pin(0, STM32_GPIO_PORTH|2))
#define PIN_H3 (::GPIO::Pin(0, STM32_GPIO_PORTH|3))
#define PIN_H4 (::GPIO::Pin(0, STM32_GPIO_PORTH|4))
#define PIN_H5 (::GPIO::Pin(0, STM32_GPIO_PORTH|5))
#define PIN_H6 (::GPIO::Pin(0, STM32_GPIO_PORTH|6))
#define PIN_H7 (::GPIO::Pin(0, STM32_GPIO_PORTH|7))
#define PIN_H8 (::GPIO::Pin(0, STM32_GPIO_PORTH|8))
#define PIN_H9 (::GPIO::Pin(0, STM32_GPIO_PORTH|9))
#define PIN_H10 (::GPIO::Pin(0, STM32_GPIO_PORTH|10))
#define PIN_H11 (::GPIO::Pin(0, STM32_GPIO_PORTH|11))
#define PIN_H12 (::GPIO::Pin(0, STM32_GPIO_PORTH|12))
#define PIN_H13 (::GPIO::Pin(0, STM32_GPIO_PORTH|13))
#define PIN_H14 (::GPIO::Pin(0, STM32_GPIO_PORTH|14))
#define PIN_H15 (::GPIO::Pin(0, STM32_GPIO_PORTH|15))
#define PIN_I0 (::GPIO::Pin(0, STM32_GPIO_PORTI|0))
#define PIN_I1 (::GPIO::Pin(0, STM32_GPIO_PORTI|1))
#define PIN_I2 (::GPIO::Pin(0, STM32_GPIO_PORTI|2))
#define PIN_I3 (::GPIO::Pin(0, STM32_GPIO_PORTI|3))
#define PIN_I4 (::GPIO::Pin(0, STM32_GPIO_PORTI|4))
#define PIN_I5 (::GPIO::Pin(0, STM32_GPIO_PORTI|5))
#define PIN_I6 (::GPIO::Pin(0, STM32_GPIO_PORTI|6))
#define PIN_I7 (::GPIO::Pin(0, STM32_GPIO_PORTI|7))
#define PIN_I8 (::GPIO::Pin(0, STM32_GPIO_PORTI|8))
#define PIN_I9 (::GPIO::Pin(0, STM32_GPIO_PORTI|9))
#define PIN_I10 (::GPIO::Pin(0, STM32_GPIO_PORTI|10))
#define PIN_I11 (::GPIO::Pin(0, STM32_GPIO_PORTI|11))
#define PIN_I12 (::GPIO::Pin(0, STM32_GPIO_PORTI|12))
#define PIN_I13 (::GPIO::Pin(0, STM32_GPIO_PORTI|13))
#define PIN_I14 (::GPIO::Pin(0, STM32_GPIO_PORTI|14))
#define PIN_I15 (::GPIO::Pin(0, STM32_GPIO_PORTI|15))
#define PIN_J0 (::GPIO::Pin(0, STM32_GPIO_PORTJ|0))
#define PIN_J1 (::GPIO::Pin(0, STM32_GPIO_PORTJ|1))
#define PIN_J2 (::GPIO::Pin(0, STM32_GPIO_PORTJ|2))
#define PIN_J3 (::GPIO::Pin(0, STM32_GPIO_PORTJ|3))
#define PIN_J4 (::GPIO::Pin(0, STM32_GPIO_PORTJ|4))
#define PIN_J5 (::GPIO::Pin(0, STM32_GPIO_PORTJ|5))
#define PIN_J6 (::GPIO::Pin(0, STM32_GPIO_PORTJ|6))
#define PIN_J7 (::GPIO::Pin(0, STM32_GPIO_PORTJ|7))
#define PIN_J8 (::GPIO::Pin(0, STM32_GPIO_PORTJ|8))
#define PIN_J9 (::GPIO::Pin(0, STM32_GPIO_PORTJ|9))
#define PIN_J10 (::GPIO::Pin(0, STM32_GPIO_PORTJ|10))
#define PIN_J11 (::GPIO::Pin(0, STM32_GPIO_PORTJ|11))
#define PIN_J12 (::GPIO::Pin(0, STM32_GPIO_PORTJ|12))
#define PIN_J13 (::GPIO::Pin(0, STM32_GPIO_PORTJ|13))
#define PIN_J14 (::GPIO::Pin(0, STM32_GPIO_PORTJ|14))
#define PIN_J15 (::GPIO::Pin(0, STM32_GPIO_PORTJ|15))
#define PIN_K0 (::GPIO::Pin(0, STM32_GPIO_PORTK|0))
#define PIN_K1 (::GPIO::Pin(0, STM32_GPIO_PORTK|1))
#define PIN_K2 (::GPIO::Pin(0, STM32_GPIO_PORTK|2))
#define PIN_K3 (::GPIO::Pin(0, STM32_GPIO_PORTK|3))
#define PIN_K4 (::GPIO::Pin(0, STM32_GPIO_PORTK|4))
#define PIN_K5 (::GPIO::Pin(0, STM32_GPIO_PORTK|5))
#define PIN_K6 (::GPIO::Pin(0, STM32_GPIO_PORTK|6))
#define PIN_K7 (::GPIO::Pin(0, STM32_GPIO_PORTK|7))
#define PIN_K8 (::GPIO::Pin(0, STM32_GPIO_PORTK|8))
#define PIN_K9 (::GPIO::Pin(0, STM32_GPIO_PORTK|9))
#define PIN_K10 (::GPIO::Pin(0, STM32_GPIO_PORTK|10))
#define PIN_K11 (::GPIO::Pin(0, STM32_GPIO_PORTK|11))
#define PIN_K12 (::GPIO::Pin(0, STM32_GPIO_PORTK|12))
#define PIN_K13 (::GPIO::Pin(0, STM32_GPIO_PORTK|13))
#define PIN_K14 (::GPIO::Pin(0, STM32_GPIO_PORTK|14))
#define PIN_K15 (::GPIO::Pin(0, STM32_GPIO_PORTK|15))


namespace STM32
{

    class __attribute__((packed,aligned(4))) GPIO final
    {
    public:
        enum Speed
        {
            SPEED_2MHZ = 0,
            SPEED_25MHZ = 1,
            SPEED_50MHZ = 2,
            SPEED_100MHZ = 3,
        };

        enum Special
        {
#include STM32_GPIO_DEF_FILE
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
            void setSpecial(unsigned int pin, enum Special function) const;
        };
        static const PinController Controller;

        static void init();

    };

}

