#pragma once

#include "global.h"


#define USB_DECLARE_STRDESC(str) \
    struct \
    { \
        USB::Descriptor::StringDescriptor header = USB::Descriptor::StringDescriptor(ARRAYLEN(str) - 1); \
        char16_t string[ARRAYLEN(str)] = str; \
    }


namespace USB
{

    enum __attribute__((packed)) EndpointType
    {
        Control = 0,
        Isochronous = 1,
        Bulk = 2,
        Interrupt = 3,
    };

    enum __attribute__((packed)) Direction
    {
        Out = 0,
        In = 1,
    };

    union __attribute__((packed)) EndpointNumber
    {
        uint8_t u8;
        struct __attribute__((packed))
        {
            uint8_t number : 4;
            uint8_t reserved : 3;
            Direction direction : 1;
        };
        constexpr EndpointNumber() : u8(0) {}
        constexpr EndpointNumber(uint8_t byte) : u8(byte) {}
        constexpr EndpointNumber(Direction direction, int number) : number(number), reserved(0), direction(direction) {}
    };

    struct __attribute__((packed)) EndpointAttributes
    {
        EndpointType type : 2;
        enum SyncType
        {
            NoSync = 0,
            Async = 1,
            Adaptive = 2,
            Sync = 3,
        } syncType : 2;
        enum Usage
        {
            Data = 0,
            Feedback = 1,
            Explicit = 2,
        } usage : 2;
        uint8_t : 2;
        constexpr EndpointAttributes(EndpointType type, SyncType syncType, Usage usage)
            : type(type), syncType(syncType), usage(usage) {}
    };

    namespace Descriptor
    {
        enum __attribute__((packed)) Type
        {
            Device = 1,
            Configuration = 2,
            String = 3,
            Interface = 4,
            Endpoint = 5,
            DeviceQualifier = 6,
            OtherSpeedConfig = 7,
            InterfacePower = 8,
            OTG = 9,
            Debug = 10,
            InterfaceAssociation = 11,
        };

        struct __attribute__((packed)) Header
        {
            uint8_t bLength;
            Type bDescriptorType;
            constexpr Header(Type type, uint8_t len) : bLength(len), bDescriptorType(type) {}
        };

        struct __attribute__((packed)) Class
        {
            uint8_t bClass;
            uint8_t bSubClass;
            uint8_t bProtocol;
            constexpr Class(uint8_t bClass, uint8_t bSubClass, uint8_t bProtocol)
                : bClass(bClass), bSubClass(bSubClass), bProtocol(bProtocol) {}
        };

        struct __attribute__((packed)) ConfigAttributes
        {
            uint8_t : 5;
            bool remoteWakeup : 1;
            bool selfPowered : 1;
            bool busPowered : 1;
            constexpr ConfigAttributes(bool busPowered, bool selfPowered, bool remoteWakeup)
                : remoteWakeup(remoteWakeup), selfPowered(selfPowered), busPowered(busPowered) {}
        };

        struct __attribute__((packed)) DeviceDescriptor : public Header
        {
            uint16_t bcdUSB;
            Class deviceClass;
            uint8_t bMaxPacketSize0;
            uint16_t idVendor;
            uint16_t idProduct;
            uint16_t bcdDevice;
            uint8_t iManufacturer;
            uint8_t iProduct;
            uint8_t iSerialNumber;
            uint8_t bNumConfigurations;
            constexpr DeviceDescriptor(uint16_t bcdUSB, Class deviceClass, uint8_t bMaxPacketSize0, uint16_t idVendor,
                                       uint16_t idProduct, uint16_t bcdDevice, uint8_t iManufacturer, uint8_t iProduct,
                                       uint8_t iSerialNumber, uint8_t bNumConfigurations)
                : Header(Device, sizeof(DeviceDescriptor)), bcdUSB(bcdUSB), deviceClass(deviceClass),
                  bMaxPacketSize0(bMaxPacketSize0), idVendor(idVendor), idProduct(idProduct), bcdDevice(bcdDevice),
                  iManufacturer(iManufacturer), iProduct(iProduct), iSerialNumber(iSerialNumber),
                  bNumConfigurations(bNumConfigurations) {}
        };

        struct __attribute__((packed)) ConfigurationDescriptor : public Header
        {
            uint16_t wTotalLength;
            uint8_t bNumInterfaces;
            uint8_t bConfigurationValue;
            uint8_t iConfiguration;
            ConfigAttributes bmAttributes;
            uint8_t bMaxPower;
            constexpr ConfigurationDescriptor(uint16_t wTotalLength, uint8_t bNumInterfaces,
                                              uint8_t bConfigurationValue, uint8_t iConfiguration,
                                              ConfigAttributes bmAttributes, uint8_t maxCurrentMa)
                : Header(Configuration, sizeof(ConfigurationDescriptor)), wTotalLength(wTotalLength),
                  bNumInterfaces(bNumInterfaces), bConfigurationValue(bConfigurationValue),
                  iConfiguration(iConfiguration), bmAttributes(bmAttributes), bMaxPower(maxCurrentMa / 2) {}
        };

        struct __attribute__((packed)) InterfaceDescriptor : public Header
        {
            uint8_t bInterfaceNumber;
            uint8_t bAlternateSetting;
            uint8_t bNumEndpoints;
            Class interfaceClass;
            uint8_t iInterface;
            constexpr InterfaceDescriptor(uint8_t bInterfaceNumber, uint8_t bAlternateSetting, uint8_t bNumEndpoints,
                                          Class interfaceClass, uint8_t iInterface)
                : Header(Interface, sizeof(InterfaceDescriptor)), bInterfaceNumber(bInterfaceNumber),
                  bAlternateSetting(bAlternateSetting), bNumEndpoints(bNumEndpoints), interfaceClass(interfaceClass),
                  iInterface(iInterface) {}
        };

        struct __attribute__((packed)) EndpointDescriptor : public Header
        {
            EndpointNumber bEndpointAddress;
            EndpointAttributes bmAttributes;
            uint16_t wMaxPacketSize;
            uint8_t bInterval;
            constexpr EndpointDescriptor(EndpointNumber bEndpointAddress, EndpointAttributes(bmAttributes),
                                         uint16_t wMaxPacketSize, uint8_t bInterval)
                : Header(Endpoint, sizeof(EndpointDescriptor)), bEndpointAddress(bEndpointAddress),
                  bmAttributes(bmAttributes), wMaxPacketSize(wMaxPacketSize), bInterval(bInterval) {}
        };

        struct __attribute__((packed)) StringDescriptor : public Header
        {
            uint16_t wString[];
            constexpr StringDescriptor(unsigned int stringLength)
                : Header(String, sizeof(StringDescriptor) + stringLength * sizeof(char16_t)), wString{} {}
        };
    }

    union __attribute__((packed)) BmRequestType
    {
        enum __attribute__((packed)) Recipient
        {
            Device = 0,
            Interface = 1,
            Endpoint = 2,
            Other = 3,
        };
        enum __attribute__((packed)) Type
        {
            Standard = 0,
            Class = 1,
            Vendor = 2,
        };
        uint8_t u8;
        struct __attribute__((packed))
        {
            enum Recipient recipient : 5;
            enum Type type : 2;
            Direction direction : 1;
        };
        constexpr BmRequestType() : recipient(Device), type(Standard), direction(Out) {}
        constexpr BmRequestType(uint8_t byte) : u8(byte) {}
        constexpr BmRequestType(Direction direction, Type type, Recipient recipient)
            : recipient(recipient), type(type), direction(direction) {}
    };

    enum __attribute__((packed)) RequestCode
    {
        GetStatus = 0,
        ClearFeature = 1,
        SetFeature = 3,
        SetAddress = 5,
        GetDescriptor = 6,
        SetDescriptor = 7,
        GetConfiguration = 8,
        SetConfiguration = 9,
        GetInterface = 10,
        SetInterface = 11,
        SynchFrame = 12,
    };

    struct __attribute__((packed,aligned(4))) SetupPacket
    {
    public:
        BmRequestType bmRequestType;
        RequestCode bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
        constexpr SetupPacket() : bmRequestType(), bRequest(GetStatus), wValue(0), wIndex(0), wLength(0) {}
        constexpr SetupPacket(BmRequestType type, RequestCode req, uint16_t wValue, uint16_t wIndex, uint16_t wLength)
            : bmRequestType(type), bRequest(req), wValue(wValue), wIndex(wIndex), wLength(wLength) {}
    };

    class USB;

    class __attribute__((packed,aligned(4))) Endpoint
    {
    public:
        virtual int ctrlRequest(USB* usb, SetupPacket* request, const void** response) = 0;
        virtual void xferComplete(USB* usb, int bytesLeft) = 0;
        virtual void setupReceived(USB* usb, uint32_t offset) = 0;
        EndpointNumber number;
        uint32_t : 24;
        constexpr Endpoint(EndpointNumber number) : number(number) {}
    };

    class __attribute__((packed,aligned(4))) AltSetting
    {
    public:
        virtual int ctrlRequest(USB* usb, SetupPacket* request, const void** response) = 0;
        virtual void set(USB* usb) = 0;
        virtual void unset(USB* usb) = 0;
        uint32_t : 24;
        uint8_t endpointCount;
        Endpoint* endpoints[];
        constexpr AltSetting(uint8_t endpointCount) : endpointCount(endpointCount), endpoints{} {}
    };

    class __attribute__((packed,aligned(4))) Interface
    {
    public:
        uint32_t : 16;
        uint8_t currentAltSetting;
        uint8_t altSettingCount;
        AltSetting* altSettings[];
        constexpr Interface(uint8_t altSettingCount)
            : currentAltSetting(0), altSettingCount(altSettingCount), altSettings{} {}
    };

    class __attribute__((packed,aligned(4))) Configuration
    {
    public:
        virtual void busReset(USB* usb) = 0;
        virtual void set(USB* usb) = 0;
        virtual void unset(USB* usb) = 0;
        const Descriptor::ConfigurationDescriptor* descriptor;
        uint32_t : 24;
        uint8_t interfaceCount;
        Interface* interfaces[];
        constexpr Configuration(const Descriptor::ConfigurationDescriptor* descriptor, uint8_t interfaceCount)
            : descriptor(descriptor), interfaceCount(interfaceCount), interfaces{} {}
    };

    class __attribute__((packed,aligned(4))) USB
    {
    private:
        virtual void drvStart() = 0;
        virtual void drvStop() = 0;
        virtual void drvEp0StartRx(bool nonSetup, int len) = 0;
        virtual void drvEp0StartTx(const void* buf, int len) = 0;
        virtual int drvGetStall(EndpointNumber ep) = 0;
        virtual void drvSetAddress(uint8_t address) = 0;
        void unconfigure();
        Endpoint* findEndpoint(EndpointNumber ep, int* ifIdx, int* epIdx);
    protected:
        void handleBusReset(int highSpeed);
        void handleXferComplete(EndpointNumber epNum, int bytesLeft);
        void handleSetupReceived(EndpointNumber epNum, uint32_t offset);
        void handleEp0Setup(SetupPacket* packet);
    public:
        void start();
        void stop();
        void ep0StartRx(bool non_setup, int len, bool (*callback)(USB* usb, EndpointNumber epNum, int bytesLeft));
        void ep0StartTx(const void* buf, int len, bool (*callback)(USB* usb, EndpointNumber epNum, int bytesLeft));
        virtual void startRx(EndpointNumber ep, void* buf, int size) = 0;
        virtual void startTx(EndpointNumber ep, const void* buf, int size) = 0;
        virtual void setStall(EndpointNumber ep, bool stall) = 0;
        virtual void configureEp(EndpointNumber ep, EndpointType type, int maxPacket) = 0;
        virtual void unconfigureEp(EndpointNumber ep) = 0;
        virtual int getMaxTransferSize(EndpointNumber ep) = 0;
        static bool ep0FullTxCallback(USB* usb, EndpointNumber epNum, int bytesLeft);
        bool ep0FullTxCallback(EndpointNumber epNum, int bytesLeft);
        static bool ep0ShortTxCallback(USB* usb, EndpointNumber epNum, int bytesLeft);
        bool ep0ShortTxCallback(EndpointNumber epNum, int bytesLeft);
        static bool ep0AckCallback(USB* usb, EndpointNumber epNum, int bytesLeft);
        bool ep0AckCallback(EndpointNumber epNum, int bytesLeft);
        union __attribute__((packed,aligned(4))) Buffer
        {
            uint8_t u8[64];
            SetupPacket setup;
            constexpr Buffer() : setup() {}
        } *buffer;
        void (*busResetHook)(USB* usb);
        int (*ctrlRequestHook)(USB* usb, SetupPacket* request, const void** response);
        int (*ep0SetupHook)(USB* usb, SetupPacket* buf);
        bool (*ep0RxCallback)(USB* usb, EndpointNumber epNum, int bytesLeft);
        bool (*ep0TxCallback)(USB* usb, EndpointNumber epNum, int bytesLeft);
        const void* ep0TxPtr;
        uint16_t ep0TxLen;
        uint8_t stringDescriptorCount;
        uint8_t configurationCount;
        const Descriptor::DeviceDescriptor* deviceDescriptor;
        const Descriptor::StringDescriptor* const* stringDescriptors;
        Configuration* const* configurations;
        uint8_t currentAddress;
        uint8_t currentConfiguration;
        uint8_t needsAlign;
        bool highSpeed;
        constexpr USB(const Descriptor::DeviceDescriptor* deviceDescriptor,
                      const Descriptor::StringDescriptor* const* stringDescriptors, uint8_t stringDescriptorCount,
                      Configuration* const* configurations, uint8_t configurationCount, void* buffer, bool needsAlign)
            : buffer((Buffer*)buffer), busResetHook(NULL), ctrlRequestHook(NULL), ep0SetupHook(NULL),
              ep0RxCallback(NULL), ep0TxCallback(NULL), ep0TxPtr(NULL), ep0TxLen(0),
              stringDescriptorCount(stringDescriptorCount), configurationCount(configurationCount),
              deviceDescriptor(deviceDescriptor), stringDescriptors(stringDescriptors), configurations(configurations),
              currentAddress(0), currentConfiguration(0), needsAlign(needsAlign), highSpeed(false) {}
    };

}
