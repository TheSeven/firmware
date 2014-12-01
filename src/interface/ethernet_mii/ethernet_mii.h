#pragma once

#include "global.h"
#include "interface/ethernet/ethernet.h"


namespace Ethernet
{

    namespace MII
    {

        enum Type
        {
            TYPE_MII = 0,
            TYPE_RMII = 1,
        };

        enum Speed
        {
            SPEED_10 = 0,
            SPEED_100 = 1,
            SPEED_1000 = 2,
        };

        enum Duplex
        {
            DUPLEX_HDX = 0,
            DUPLEX_FDX = 1,
        };

        enum Wiring
        {
            WIRING_MDI = 0,
            WIRING_MDIX = 1,
        };

        class __attribute__((packed,aligned(4))) Driver;

        class __attribute__((packed,aligned(4))) MAC
        {
        public:
            virtual enum Result update(Interface* intf, Driver* driver) = 0;
            virtual enum Result phyRead(int phyid, int reg, uint16_t* data) = 0;
            virtual enum Result phyWrite(int phyid, int reg, uint16_t data) = 0;
            virtual err_t lwipInit(Interface* intf, Driver* driver) = 0;
            virtual err_t lwipTx(struct pbuf *p) = 0;
        };

        class __attribute__((packed,aligned(4))) PHY
        {
        public:
            virtual enum Result sense(Interface* intf, Driver* driver) = 0;
        };

        class __attribute__((packed,aligned(4))) Driver final : public Ethernet::Driver
        {
        public:
            constexpr Driver(MAC* mac, PHY* phy)
                : mac(mac), phy(phy), speed(SPEED_10), duplex(DUPLEX_HDX), wiring(WIRING_MDI) {}
            virtual enum Result update(Interface* intf);
            virtual err_t lwipInit(Interface* intf);
            virtual err_t lwipTx(struct pbuf *p);

            MAC* mac;
            PHY* phy;
            enum Speed speed : 2;
            enum Duplex duplex : 1;
            enum Wiring wiring : 1;
        };

    }

}
