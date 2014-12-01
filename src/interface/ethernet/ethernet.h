#pragma once

#include "global.h"
#include "lib/lwip/lwip.h"


namespace Ethernet
{

    enum Result
    {
        RESULT_OK = 0,
        RESULT_UNKNOWN_ERROR,
        RESULT_UNSUPPORTED,
        RESULT_NOT_AVAILABLE,
        RESULT_INVALID_ARGUMENT,
    };

    class __attribute__((packed,aligned(4))) Interface;

    class __attribute__((packed,aligned(4))) Driver
    {
    public:
        virtual enum Result update(Interface* intf) = 0;
        virtual err_t lwipInit(Interface* intf) = 0;
        virtual err_t lwipTx(struct pbuf *p) = 0;
    };

    class __attribute__((packed,aligned(4))) Interface final
    {
    private:
        static err_t initNetIf(struct netif *netif);
        static err_t lwipTx(struct netif *netif, struct pbuf *p);

        static bool lwipInitialized;

    public:
        Interface(Driver* driver, uint16_t mtu, uint8_t* macaddr);
        enum Result tick();

        Driver* driver;
        struct netif lwipIf;
        bool linkUp : 1;
        bool stateChanged : 1;
    };

}
