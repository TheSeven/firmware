#pragma once

#include "global.h"
#include "interface/uart/uart.h"


namespace STM32
{

    class __attribute__((packed,aligned(4))) UART final : public ::UART::Bytewise
    {
    private:
        uint8_t index;

        constexpr UART(int index) : index(index) {}

    public:
        class __attribute__((packed)) Config
        {
        public:
            enum StopBits
            {
                StopBits1_0 = 0,
                StopBits0_5 = 1,
                StopBits2_0 = 2,
                StopBits1_5 = 3,
            } stopBits : 2;
            enum Parity
            {
                ParityNone = 0,
                ParityEven = 1,
                ParityOdd = 2,
            } parity : 2;
            bool honorCTS : 1;
            bool signalRTS : 1;
            bool halfDuplex : 1;
        };

        void bwTx(uint8_t byte);
        void flush();

        enum ::UART::Result configure(int baudrate, int tolerance, Config config);
        void irq();

        static UART UART1;
        static UART UART2;
        static UART UART3;
        static UART UART4;
        static UART UART5;
        static UART UART6;
        static UART UART7;
        static UART UART8;
    };

}

