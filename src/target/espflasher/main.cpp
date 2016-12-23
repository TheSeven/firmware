#include "global.h"
#include "app/main.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/uart.h"
#include "soc/stm32/f0/usb.h"
#include "sys/time.h"
#include "sys/util.h"
#include "lib/ringbuffer/ringbuffer.h"


namespace USB
{

    namespace CDC
    {

        enum __attribute__((packed)) Request
        {
            SetLineCoding = 0x20,
            GetLineCoding = 0x21,
            SetControlLineState = 0x22,
        };

        namespace Descriptor
        {

            enum __attribute__((packed)) Type
            {
                CDCDescriptor = 0x24,
            };

            enum __attribute__((packed)) SubType
            {
                CDCHeader = 0x00,
                CDCCallManagement = 0x01,
                CDCAbstractControlManagement = 0x02,
                CDCUnion = 0x06,
            };

            struct __attribute__((packed)) CDCDescHeader : public ::USB::Descriptor::Header
            {
                SubType subType : 8;
                constexpr CDCDescHeader(SubType subType, uint8_t length)
                    : ::USB::Descriptor::Header((::USB::Descriptor::Type)CDCDescriptor, length),
                      subType(subType) {}
            };

            struct __attribute__((packed)) Header : public CDCDescHeader
            {
                uint16_t bcdCDC;
                constexpr Header(uint16_t bcdCDC) : CDCDescHeader(CDCHeader, sizeof(Header)), bcdCDC(bcdCDC) {}
            };

            struct __attribute__((packed)) CallManagement : public CDCDescHeader
            {
                uint8_t bmCapabilities;
                uint8_t bDataInterface;
                constexpr CallManagement(uint8_t bmCapabilities, uint8_t bDataInterface)
                    : CDCDescHeader(CDCCallManagement, sizeof(CallManagement)),
                      bmCapabilities(bmCapabilities), bDataInterface(bDataInterface) {}
            };

            struct __attribute__((packed)) AbstractControlManagement : public CDCDescHeader
            {
                uint8_t bmCapabilities;
                constexpr AbstractControlManagement(uint8_t bmCapabilities)
                    : CDCDescHeader(CDCAbstractControlManagement, sizeof(AbstractControlManagement)),
                      bmCapabilities(bmCapabilities) {}
            };

            struct __attribute__((packed)) Union : public CDCDescHeader
            {
                uint8_t bMasterInterface;
                uint8_t bSlaveInterface0;
                constexpr Union(uint8_t bMasterInterface, uint8_t bSlaveInterface0)
                    : CDCDescHeader(CDCUnion, sizeof(Union)),
                      bMasterInterface(bMasterInterface), bSlaveInterface0(bSlaveInterface0) {}
            };

            struct __attribute__((packed)) InterfaceAssociation : public ::USB::Descriptor::Header
            {
                uint8_t bFirstInterface;
                uint8_t bInterfaceCount;
                ::USB::Descriptor::Class functionClass;
                uint8_t iInterface;
                constexpr InterfaceAssociation(uint8_t bFirstInterface, uint8_t bInterfaceCount,
                                               ::USB::Descriptor::Class functionClass, uint8_t iInterface)
                    : ::USB::Descriptor::Header(::USB::Descriptor::InterfaceAssociation, sizeof(InterfaceAssociation)),
                      bFirstInterface(bFirstInterface), bInterfaceCount(bInterfaceCount),
                      functionClass(functionClass), iInterface(iInterface) {}
            };

        }

    }

}

struct __attribute__((packed)) LineCoding
{
    uint32_t baudRate;
    uint8_t stopBits;
    uint8_t parityType;
    uint8_t dataBits;
};
static LineCoding lineCoding{115200, 0, 0,  8};
static LineCoding debugLineCoding{115200, 0, 0,  8};
static bool lineCodingChange = true;
static bool handleSetLineCoding(USB::USB* usb, USB::EndpointNumber epNum, int bytesLeft);
static bool handleDebugSetLineCoding(USB::USB* usb, USB::EndpointNumber epNum, int bytesLeft);

class __attribute__((aligned(4))) DataInterface : public USB::Interface
{
public:
    class __attribute__((aligned(4))) DataAltsetting : public USB::AltSetting
    {
    private:
        class __attribute__((aligned(4))) DataOutEndpoint : public USB::Endpoint
        {
            RingBuffer<uint8_t, 256> txBuf;
            uint8_t rxBlock[64];
            uint8_t pendingBytes;
            bool rxActive;
            bool txActive;

        public:
            bool connected;

            void startRx(USB::USB* usb)
            {
                if (!connected || rxActive || lineCodingChange) return;
                if (txBuf.free() < 64) return;
                rxActive = true;
                usb->startRx(number, rxBlock, sizeof(rxBlock));
            }

            void sendNext(USB::USB* usb)
            {
                uint8_t byte;
                if (!txBuf.get(&byte)) txActive = false;
                else STM32::UART::UART1.bwTx(byte);
                startRx(usb);
            }

            void resumeTx(USB::USB* usb)
            {
                if (txActive) return;
                if (lineCodingChange)
                {
                    lineCodingChange = false;
                    STM32::UART::UART1.flush();
                    STM32::UART::UART1.configure(lineCoding.baudRate, 1000000,
                                                 {STM32::UART::Config::StopBits1_0, STM32::UART::Config::ParityNone,
                                                  false, false, false});
                    resumePending(usb);
                }
                uint8_t byte;
                if (!txBuf.get(&byte)) return;
                txActive = true;
                STM32::UART::UART1.bwTx(byte);
            }

            constexpr DataOutEndpoint(int number)
                : Endpoint(USB::EndpointNumber(USB::Out, number)),
                  txBuf(), rxBlock{}, pendingBytes(0), rxActive(false), txActive(false), connected(false) {}

        private:
            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                return -1;
            }

            void resumePending(USB::USB* usb)
            {
                if (lineCodingChange) return;
                if (pendingBytes) txBuf.put(rxBlock, pendingBytes);
                pendingBytes = 0;
                startRx(usb);
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
                pendingBytes = sizeof(rxBlock) - bytesLeft;
                rxActive = false;
                resumePending(usb);
                resumeTx(usb);
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }
        };

        class __attribute__((aligned(4))) DataInEndpoint : public USB::Endpoint
        {
            RingBuffer<uint8_t, 256> rxBuf;
            uint8_t txBlock[64];
            bool txActive;

        public:
            bool connected;

            void putByte(USB::USB* usb, uint8_t byte)
            {
                if (!connected) return;
                rxBuf.put(&byte);
                startTx(usb);
            }

            void flush()
            {
                rxBuf.flush();
            }

            constexpr DataInEndpoint(int number)
                : Endpoint(USB::EndpointNumber(USB::In, number)),
                  rxBuf(), txBlock{}, txActive(false), connected(false) {}

            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                return -1;
            }

            void startTx(USB::USB* usb)
            {
                if (txActive) return;
                int len = rxBuf.get(txBlock, sizeof(txBlock));
                if (!len) return;
                txActive = true;
                usb->startTx(number, txBlock, len);
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
                txActive = false;
                startTx(usb);
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }
        };

    public:
        const USB::Endpoint* const endpoints[2];
        DataOutEndpoint outEp;
        DataInEndpoint inEp;

    private:
        int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
        {
            return -1;
        }

        void set(USB::USB* usb)
        {
            usb->configureEp(outEp.number, USB::Bulk, 64);
            usb->configureEp(inEp.number, USB::Bulk, 64);
            lineCodingChange = false;
            inEp.flush();
            outEp.connected = true;
            inEp.connected = true;
            outEp.startRx(usb);
        }

        void unset(USB::USB* usb)
        {
            outEp.connected = false;
            inEp.connected = false;
            usb->unconfigureEp(outEp.number);
            usb->unconfigureEp(inEp.number);
        }

    public:
        constexpr DataAltsetting(int outEpNum, int inEpNum)
            : AltSetting(ARRAYLEN(endpoints)), endpoints{&outEp, &inEp}, outEp(outEpNum), inEp(inEpNum) {}
    };

    const USB::AltSetting* const altSettings[1];
    DataAltsetting altSetting;

public:
    constexpr DataInterface(int outEpNum, int inEpNum)
        : Interface(ARRAYLEN(altSettings)), altSettings{&altSetting}, altSetting(outEpNum, inEpNum) {}
};

class __attribute__((aligned(4))) ACMInterface : public USB::Interface
{
public:
    class __attribute__((aligned(4))) ACMAltsetting : public USB::AltSetting
    {
    private:
        class __attribute__((aligned(4))) ACMEndpoint : public USB::Endpoint
        {
            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                return -1;
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }

        public:
            constexpr ACMEndpoint(int number) : Endpoint(USB::EndpointNumber(USB::In, number)) {}
        };

        const USB::Endpoint* const endpoints[1];
        ACMEndpoint acmEp;

        int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
        {
            int size = -1;
            switch (request->bmRequestType.type)
            {
            case USB::BmRequestType::Class:
                switch ((USB::CDC::Request)request->bRequest)
                {
                case USB::CDC::GetLineCoding:
                    if (request->wLength != sizeof(lineCoding)) break;
                    *response = &lineCoding;
                    size = sizeof(lineCoding);
                    break;
                case USB::CDC::SetLineCoding:
                    if (request->wLength != sizeof(lineCoding)) break;
                    usb->ep0StartRx(true, sizeof(lineCoding), &handleSetLineCoding);
                    size = -3;
                    break;
                case USB::CDC::SetControlLineState:
                    if (request->wLength) break;
                    GPIO::setLevel(PIN_B1, request->wValue & 1);
                    GPIO::setLevel(PIN_B2, request->wValue & 2);
                    size = 0;
                    break;
                default: break;
                }
                break;
            }
            return size;
        }

        void set(USB::USB* usb)
        {
            usb->configureEp(acmEp.number, USB::Interrupt, 64);
            GPIO::setLevel(PIN_B1, false);
            GPIO::setLevel(PIN_B2, true);
        }

        void unset(USB::USB* usb)
        {
            usb->unconfigureEp(acmEp.number);
            GPIO::setLevel(PIN_B2, true);
            GPIO::setLevel(PIN_B1, true);
        }

    public:
        constexpr ACMAltsetting(int acmEpNum)
            : AltSetting(ARRAYLEN(endpoints)), endpoints{&acmEp}, acmEp(acmEpNum) {}
    };

    const USB::AltSetting* const altSettings[1];
    ACMAltsetting altSetting;

public:
    constexpr ACMInterface(int acmEpNum)
        : Interface(ARRAYLEN(altSettings)), altSettings{&altSetting}, altSetting(acmEpNum) {}
};

class __attribute__((aligned(4))) DebugDataInterface : public USB::Interface
{
public:
    class __attribute__((aligned(4))) DataAltsetting : public USB::AltSetting
    {
    private:
        class __attribute__((aligned(4))) DataOutEndpoint : public USB::Endpoint
        {
            uint8_t rxBlock[64];

        public:
            bool connected;

            void startRx(USB::USB* usb)
            {
                if (!connected) return;
                usb->startRx(number, rxBlock, sizeof(rxBlock));
            }

            constexpr DataOutEndpoint(int number)
                : Endpoint(USB::EndpointNumber(USB::Out, number)), rxBlock{}, connected(false) {}

            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                return -1;
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
                startRx(usb);
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }
        };

        class __attribute__((aligned(4))) DataInEndpoint : public USB::Endpoint
        {
            RingBuffer<uint8_t, 256> rxBuf;
            uint8_t txBlock[64];
            bool txActive;

        public:
            bool connected;

            void putByte(USB::USB* usb, uint8_t byte)
            {
                if (!connected) return;
                rxBuf.put(&byte);
                startTx(usb);
            }

            void flush()
            {
                rxBuf.flush();
            }

            constexpr DataInEndpoint(int number)
                : Endpoint(USB::EndpointNumber(USB::In, number)),
                  rxBuf(), txBlock{}, txActive(false), connected(false) {}

            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                return -1;
            }

            void startTx(USB::USB* usb)
            {
                if (txActive) return;
                int len = rxBuf.get(txBlock, sizeof(txBlock));
                if (!len) return;
                txActive = true;
                usb->startTx(number, txBlock, len);
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
                txActive = false;
                startTx(usb);
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }
        };

    public:
        const USB::Endpoint* const endpoints[2];
        DataOutEndpoint outEp;
        DataInEndpoint inEp;

    private:
        int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
        {
            return -1;
        }

        void set(USB::USB* usb)
        {
            usb->configureEp(outEp.number, USB::Bulk, 64);
            usb->configureEp(inEp.number, USB::Bulk, 64);
            inEp.flush();
            outEp.connected = true;
            inEp.connected = true;
            outEp.startRx(usb);
        }

        void unset(USB::USB* usb)
        {
            outEp.connected = false;
            inEp.connected = false;
            usb->unconfigureEp(outEp.number);
            usb->unconfigureEp(inEp.number);
        }

    public:
        constexpr DataAltsetting(int outEpNum, int inEpNum)
            : AltSetting(ARRAYLEN(endpoints)), endpoints{&outEp, &inEp}, outEp(outEpNum), inEp(inEpNum) {}
    };

    const USB::AltSetting* const altSettings[1];
    DataAltsetting altSetting;

public:
    constexpr DebugDataInterface(int outEpNum, int inEpNum)
        : Interface(ARRAYLEN(altSettings)), altSettings{&altSetting}, altSetting(outEpNum, inEpNum) {}
};

class __attribute__((aligned(4))) DebugACMInterface : public USB::Interface
{
public:
    class __attribute__((aligned(4))) ACMAltsetting : public USB::AltSetting
    {
    private:
        class __attribute__((aligned(4))) ACMEndpoint : public USB::Endpoint
        {
            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                return -1;
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }

        public:
            constexpr ACMEndpoint(int number) : Endpoint(USB::EndpointNumber(USB::In, number)) {}
        };

        const USB::Endpoint* const endpoints[1];
        ACMEndpoint acmEp;

        int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
        {
            int size = -1;
            switch (request->bmRequestType.type)
            {
            case USB::BmRequestType::Class:
                switch ((USB::CDC::Request)request->bRequest)
                {
                case USB::CDC::GetLineCoding:
                    if (request->wLength != sizeof(debugLineCoding)) break;
                    *response = &debugLineCoding;
                    size = sizeof(debugLineCoding);
                    break;
                case USB::CDC::SetLineCoding:
                    if (request->wLength != sizeof(debugLineCoding)) break;
                    usb->ep0StartRx(true, sizeof(debugLineCoding), &handleDebugSetLineCoding);
                    size = -3;
                    break;
                case USB::CDC::SetControlLineState:
                    if (request->wLength) break;
                    size = 0;
                    break;
                default: break;
                }
                break;
            }
            return size;
        }

        void set(USB::USB* usb)
        {
            usb->configureEp(acmEp.number, USB::Interrupt, 64);
        }

        void unset(USB::USB* usb)
        {
            usb->unconfigureEp(acmEp.number);
        }

    public:
        constexpr ACMAltsetting(int acmEpNum)
            : AltSetting(ARRAYLEN(endpoints)), endpoints{&acmEp}, acmEp(acmEpNum) {}
    };

    const USB::AltSetting* const altSettings[1];
    ACMAltsetting altSetting;

public:
    constexpr DebugACMInterface(int acmEpNum)
        : Interface(ARRAYLEN(altSettings)), altSettings{&altSetting}, altSetting(acmEpNum) {}
};

enum InterfaceNumber
{
    ControlInterface = 0,
    BulkInterface,
    DebugControlInterface,
    DebugBulkInterface,
    InterfaceCount
};

enum OutEndpoint
{
    StandardControlOutEndpoint = 0,
    DummyOutEndpoint,
    DataOutEndpoint,
    DebugDummyOutEndpoint,
    DebugDataOutEnpoint,
    OutEndpointCount
};

enum InEndpoint
{
    StandardControlInEndpoint = 0,
    DataInEndpoint,
    InterruptInEndpoint,
    DebugDataInEndpoint,
    DebugInterruptInEndpoint,
    InEndpointCount
};

class __attribute__((aligned(4))) CDCConfiguration : public USB::Configuration
{
    static constexpr const struct __attribute__((packed)) _config_descriptor
    {
        USB::Descriptor::ConfigurationDescriptor cfg;
        USB::CDC::Descriptor::InterfaceAssociation f0_iad;
        USB::Descriptor::InterfaceDescriptor i0_a0;
        USB::CDC::Descriptor::Header i0_a0_cdc_header;
        USB::CDC::Descriptor::CallManagement i0_a0_cdc_cm;
        USB::CDC::Descriptor::AbstractControlManagement i0_a0_cdc_acm;
        USB::CDC::Descriptor::Union i0_a0_cdc_union;
        USB::Descriptor::EndpointDescriptor i0_a0_eps[1];
        USB::Descriptor::InterfaceDescriptor i1_a0;
        USB::Descriptor::EndpointDescriptor i1_a0_eps[2];
        USB::CDC::Descriptor::InterfaceAssociation f1_iad;
        USB::Descriptor::InterfaceDescriptor i2_a0;
        USB::CDC::Descriptor::Header i2_a0_cdc_header;
        USB::CDC::Descriptor::CallManagement i2_a0_cdc_cm;
        USB::CDC::Descriptor::AbstractControlManagement i2_a0_cdc_acm;
        USB::CDC::Descriptor::Union i2_a0_cdc_union;
        USB::Descriptor::EndpointDescriptor i2_a0_eps[1];
        USB::Descriptor::InterfaceDescriptor i3_a0;
        USB::Descriptor::EndpointDescriptor i3_a0_eps[2];
    } descriptor
    {
        USB::Descriptor::ConfigurationDescriptor(sizeof(struct _config_descriptor), InterfaceCount, 1, 0,
                                                 USB::Descriptor::ConfigAttributes(true, true, false), 100),
        USB::CDC::Descriptor::InterfaceAssociation(ControlInterface, 2, USB::Descriptor::Class(0x02, 0x02, 0x01), 0),
        USB::Descriptor::InterfaceDescriptor(ControlInterface, 0, ARRAYLEN(_config_descriptor::i0_a0_eps),
                                             USB::Descriptor::Class(0x02, 0x02, 0x01), 0),
        USB::CDC::Descriptor::Header(0x0110),
        USB::CDC::Descriptor::CallManagement(0x01, BulkInterface),
        USB::CDC::Descriptor::AbstractControlManagement(0x06),
        USB::CDC::Descriptor::Union(ControlInterface, BulkInterface),
        {
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::In, InterruptInEndpoint),
                                                USB::EndpointAttributes(USB::Interrupt, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 64),
        },
        USB::Descriptor::InterfaceDescriptor(BulkInterface, 0, ARRAYLEN(_config_descriptor::i1_a0_eps),
                                             USB::Descriptor::Class(0x0a, 0x00, 0x00), 0),
        {
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::Out, DataOutEndpoint),
                                                USB::EndpointAttributes(USB::Bulk, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 0),
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::In, DataInEndpoint),
                                                USB::EndpointAttributes(USB::Bulk, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 0),
        },
        USB::CDC::Descriptor::InterfaceAssociation(DebugControlInterface, 2,
                                                   USB::Descriptor::Class(0x02, 0x02, 0x01), 0),
        USB::Descriptor::InterfaceDescriptor(DebugControlInterface, 0, ARRAYLEN(_config_descriptor::i2_a0_eps),
                                             USB::Descriptor::Class(0x02, 0x02, 0x01), 0),
        USB::CDC::Descriptor::Header(0x0110),
        USB::CDC::Descriptor::CallManagement(0x01, DebugBulkInterface),
        USB::CDC::Descriptor::AbstractControlManagement(0x06),
        USB::CDC::Descriptor::Union(DebugControlInterface, DebugBulkInterface),
        {
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::In, DebugInterruptInEndpoint),
                                                USB::EndpointAttributes(USB::Interrupt, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 64),
        },
        USB::Descriptor::InterfaceDescriptor(DebugBulkInterface, 0, ARRAYLEN(_config_descriptor::i3_a0_eps),
                                             USB::Descriptor::Class(0x0a, 0x00, 0x00), 0),
        {
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::Out, DebugDataOutEnpoint),
                                                USB::EndpointAttributes(USB::Bulk, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 0),
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::In, DebugDataInEndpoint),
                                                USB::EndpointAttributes(USB::Bulk, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 0),
        },
    };

public:
    const USB::Interface* const interfaces[4];
    ACMInterface acmInterface;
    DataInterface dataInterface;
    DebugACMInterface debugAcmInterface;
    DebugDataInterface debugDataInterface;

private:
    void busReset(USB::USB* usb)
    {
    }

    void set(USB::USB* usb)
    {
    }

    void unset(USB::USB* usb)
    {
    }

public:
    constexpr CDCConfiguration()
        : Configuration(&descriptor.cfg, ARRAYLEN(interfaces)),
          interfaces{&acmInterface, &dataInterface, &debugAcmInterface, &debugDataInterface},
          acmInterface(InterruptInEndpoint), dataInterface(DataOutEndpoint, DataInEndpoint),
          debugAcmInterface(DebugInterruptInEndpoint), debugDataInterface(DebugDataOutEnpoint, DebugDataInEndpoint) {}
};
constexpr const CDCConfiguration::_config_descriptor CDCConfiguration::descriptor;

static CDCConfiguration cdcConfig{};

static USB::Configuration* const usbConfigs[] =
{
    &cdcConfig,
};

static const USB::Descriptor::DeviceDescriptor usbDevDesc(0x200, USB::Descriptor::Class(0xef, 2, 1), 64,
                                                          0xffff, 0x8266, 0x0100, 1, 2, 3, ARRAYLEN(usbConfigs));

static const USB_DECLARE_STRDESC(u"\x0409") usbStrLanguage;
static const USB_DECLARE_STRDESC(u"Michael Sparmann") usbStrVendor;
static const USB_DECLARE_STRDESC(u"ESP8266 Flasher") usbStrProduct;
static const USB_DECLARE_STRDESC(u"") usbStrSerial;
static const USB::Descriptor::StringDescriptor* const usbStrDescs[] =
{
    &usbStrLanguage.header,
    &usbStrVendor.header,
    &usbStrProduct.header,
    &usbStrSerial.header,
};

static STM32::USB usb(&usbDevDesc, usbStrDescs, ARRAYLEN(usbStrDescs), usbConfigs, ARRAYLEN(usbConfigs));

static bool handleSetLineCoding(USB::USB* usb, USB::EndpointNumber epNum, int bytesLeft)
{
    if (!bytesLeft)
    {
        memcpy(&lineCoding, usb->buffer->u8, sizeof(lineCoding));
        lineCodingChange = true;
        cdcConfig.dataInterface.altSetting.outEp.resumeTx(usb);
    }
    usb->setStall(epNum, true);
    usb->ep0StartTx(NULL, 0, NULL);
    return false;
}

static bool handleDebugSetLineCoding(USB::USB* usb, USB::EndpointNumber epNum, int bytesLeft)
{
    if (!bytesLeft)
    {
        memcpy(&debugLineCoding, usb->buffer->u8, sizeof(debugLineCoding));
        STM32::UART::UART2.configure(debugLineCoding.baudRate, 1000000,
                                     {STM32::UART::Config::StopBits1_0, STM32::UART::Config::ParityNone,
                                      false, false, false});
    }
    usb->setStall(epNum, true);
    usb->ep0StartTx(NULL, 0, NULL);
    return false;
}

class UARTHandler : public UART::BytewiseHandler
{
    virtual void bwTxCallback()
    {
        cdcConfig.dataInterface.altSetting.outEp.sendNext(&usb);
    }

    virtual void bwRxCallback(uint8_t byte, enum UART::Result status)
    {
        cdcConfig.dataInterface.altSetting.inEp.putByte(&usb, byte);
    }

public:
    UARTHandler(UART::Bytewise* uart) : BytewiseHandler(uart) {}
};

static UARTHandler uartHandler(&STM32::UART::UART1);

class DebugUARTHandler : public UART::BytewiseHandler
{
    virtual void bwTxCallback()
    {
    }

    virtual void bwRxCallback(uint8_t byte, enum UART::Result status)
    {
        cdcConfig.debugDataInterface.altSetting.inEp.putByte(&usb, byte);
    }

public:
    DebugUARTHandler(UART::Bytewise* uart) : BytewiseHandler(uart) {}
};

static DebugUARTHandler debugUartHandler(&STM32::UART::UART2);


int main()
{
    usb.start();
    while (true) idle();
}
