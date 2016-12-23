#pragma once

#include "global.h"
#include "interface/usb/usb.h"
#include "core/dwotg/regs.h"


class __attribute__((packed,aligned(4))) DWOTG : public USB::USB
{
    volatile DWOTGRegs::core_regs* regs __attribute__((aligned(4)));
protected:
    bool phy16bit : 1;
    bool phyUlpi : 1;
    bool useDma : 1;
    bool sharedTxFifo : 1;
    bool disableDoubleBuffering : 1;
    uint32_t : 3;
    uint8_t fifoCount;
    uint16_t totalFifoSize;
    const uint16_t* fifoSizeList;
    Buffer buffer;
    struct __attribute__((packed,aligned(4))) EndpointDataPtr
    {
        uint32_t* rxAddr;
        const uint32_t* txAddr;
        constexpr EndpointDataPtr() : rxAddr(0), txAddr(0) {}
    } endpoints[];
private:
    virtual void socEnableClocks() = 0;
    virtual void socDisableClocks() = 0;
    virtual void socEnableIrq() = 0;
    virtual void socDisableIrq() = 0;
    virtual void socClearIrq() = 0;
    void drvStart();
    void drvStop();
    void drvEp0StartRx(bool nonSetup, int len);
    void drvEp0StartTx(const void* buf, int len);
    int drvGetStall(::USB::EndpointNumber ep);
    void drvSetAddress(uint8_t address);
    void startRx(::USB::EndpointNumber ep, void* buf, int size);
    void startTx(::USB::EndpointNumber ep, const void* buf, int size);
    void setStall(::USB::EndpointNumber ep, bool stall);
    void configureEp(::USB::EndpointNumber ep, ::USB::EndpointType type, int maxPacket);
    void unconfigureEp(::USB::EndpointNumber ep);
    int getMaxTransferSize(::USB::EndpointNumber ep);
    void flushOutEndpoint(int ep);
    void flushInEndpoint(int ep);
    void flushIrqs();
    void tryPush(int ep);
    void ep0Init();
public:
    void handleIrqInternal();
    constexpr DWOTG(const ::USB::Descriptor::DeviceDescriptor* deviceDescriptor,
                    const ::USB::Descriptor::StringDescriptor* const* stringDescriptors, uint8_t stringDescriptorCount,
                    ::USB::Configuration* const* configurations, uint8_t configurationCount,
                    volatile DWOTGRegs::core_regs* regs, bool phy16bit, bool phyUlpi, bool useDma, bool sharedTxFifo,
                    bool disableDoubleBuffering, uint8_t fifoCount, uint16_t totalFifoSize, uint16_t* fifoSizeList)
        : USB(deviceDescriptor, stringDescriptors, stringDescriptorCount,
              configurations, configurationCount, &buffer, true),
          regs(regs), phy16bit(phy16bit), phyUlpi(phyUlpi), useDma(useDma), sharedTxFifo(sharedTxFifo),
          disableDoubleBuffering(disableDoubleBuffering), fifoCount(fifoCount), totalFifoSize(totalFifoSize),
          fifoSizeList(fifoSizeList), endpoints() {}
};

