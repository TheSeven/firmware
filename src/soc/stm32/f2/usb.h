#pragma once

#include "global.h"
#include "core/dwotg/dwotg.h"
#include "soc/stm32/f2/clockgates.h"
#include "cpu/arm/cortexm/irq.h"


namespace STM32
{

    enum UsbCore
    {
        OTG_FS = 0,
        OTG_HS = 1,
        USB_CORE_COUNT,
    };

    class __attribute__((packed,aligned(4))) USB : public DWOTG
    {
        void socEnableClocks();
        void socDisableClocks();
        void socEnableIrq();
        void socDisableIrq();
        void socClearIrq();
        EndpointDataPtr endpoints[4];
        UsbCore core;

        static constexpr struct __attribute__((packed,aligned(4))) CoreParams
        {
            volatile DWOTGRegs::core_regs* regs;
            uint16_t totalFifoSize;
            bool phy16bit : 1;
            bool phyUlpi : 1;
            bool useDma : 1;
            bool sharedTxFifo : 1;
            uint32_t : 12;
        } coreParams[] =
        {
            { (volatile DWOTGRegs::core_regs*)0x50000000, 320, false, false, false, false },
            { (volatile DWOTGRegs::core_regs*)0x40040000, 1024, false, false, true, false },
        };

        static constexpr struct __attribute__((packed,aligned(2))) CoreRuntimeParams
        {
            uint8_t irq;
            uint8_t clkgate;
        } coreRuntimeParams[] =
        {
            { otg_fs_IRQn, STM32_OTGFS_CLOCKGATE },
            { otg_hs_IRQn, STM32_OTGHS_CLOCKGATE },
        };

        static USB* activeInstance[USB_CORE_COUNT];

    public:
        static void handleIrq(UsbCore core);
        constexpr USB(const ::USB::Descriptor::DeviceDescriptor* deviceDescriptor,
                      const ::USB::Descriptor::BOSDescriptor* bosDescriptor,
                      const ::USB::Descriptor::StringDescriptor* const* stringDescriptors,
                      uint8_t stringDescriptorCount, ::USB::Configuration* const* configurations,
                      uint8_t configurationCount, UsbCore core, const uint16_t* fifoSizeList, unsigned int fifoCount)
            : DWOTG(deviceDescriptor, bosDescriptor, stringDescriptors, stringDescriptorCount, configurations,
                    configurationCount, coreParams[core].regs, coreParams[core].phy16bit, coreParams[core].phyUlpi,
                    coreParams[core].useDma, coreParams[core].sharedTxFifo, false, fifoCount,
                    coreParams[core].totalFifoSize, fifoSizeList), core(core) {}
    };

}
