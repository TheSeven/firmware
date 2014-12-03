#include "global.h"
#include "app/main.h"
#include "target/artnetnode/configstore.h"
#include "target/artnetnode/network.h"
#include "target/artnetnode/dmx.h"
#include "target/artnetnode/misc.h"
#include "sys/util.h"


int main()
{
    earlyInit();
    initConfig();
    initDMX();
    initNetwork();
    lateInit();

    dhcp_start(&netIf.lwipIf);

    while (true) doBackgroundWork();
}
