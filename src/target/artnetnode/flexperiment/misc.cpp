#include "global.h"
#include "target/artnetnode/misc.h"
#include "target/artnetnode/network.h"
#include "target/artnetnode/dmx.h"
#include "sys/util.h"
#include "sys/time.h"


void earlyInit()
{
}


void lateInit()
{
}


extern void doBackgroundWork()
{
    udelay(500000); //TODO: idle();
    pollDMX();
    netIf.tick();
}
