#include "global.h"
#include "target/artnetnode/misc.h"
#include "target/artnetnode/configstore.h"
#include "target/artnetnode/network.h"
#include "target/artnetnode/dmx.h"
#include "soc/stm32/iwdg_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "interface/irq/irq.h"
#include "sys/util.h"
#include "sys/time.h"


#ifndef NET_TICK_INTERVAL
#define NET_TICK_INTERVAL 100000
#endif

#ifndef DMX_RESEND_INTERVAL
#define DMX_RESEND_INTERVAL 500000
#endif


int nextNetTick;
int nextResendTick;


void earlyInit()
{
    STM32_IWDG_REGS.KR = 0xcccc;
    irq_set_priority_grouping(0);
    irq_set_priority(SysTick_IRQn, 8);
    irq_set_priority(eth_IRQn, 4);
}


void lateInit()
{
    STM32_IWDG_REGS.KR = 0xaaaa;
    nextNetTick = TIMEOUT_SETUP(0);
    nextResendTick = TIMEOUT_SETUP(DMX_RESEND_INTERVAL * 2);
}


#include "soc/stm32/uart_regs.h"
extern void doBackgroundWork()
{
    if (configChanged)
    {
        STM32_IWDG_REGS.KR = 0xaaaa;
        saveConfig();
        STM32_IWDG_REGS.KR = 0xaaaa;
    }
    if (TIMEOUT_EXPIRED(nextNetTick))
    {
        netIf.tick();
        nextNetTick = TIMEOUT_SETUP(NET_TICK_INTERVAL);
        STM32_IWDG_REGS.KR = 0xaaaa;
    }
    if (TIMEOUT_EXPIRED(nextResendTick))
    {
        for (uint32_t i = 0; i < ARRAYLEN(dmxOutChannel); i++)
            if (TIMEOUT_EXPIRED(dmxOutChannel[i]->lastSentTime + DMX_RESEND_INTERVAL))
                dmxOutChannel[i]->sendPacket();
        nextResendTick = TIMEOUT_SETUP(DMX_RESEND_INTERVAL >> 2);
    }
}
