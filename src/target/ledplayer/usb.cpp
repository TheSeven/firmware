#include "global.h"
#include "app/main.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/f0/usb.h"
#include "sys/util.h"
#include "common.h"
#include "scsi.h"


class __attribute__((aligned(4))) UMSInterface : public USB::Interface
{
public:
    class __attribute__((aligned(4))) UMSAltSetting : public USB::AltSetting
    {
    private:
        class __attribute__((aligned(4))) UMSOutEndpoint : public USB::Endpoint
        {
            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                int size = -1;
                switch (request->bmRequestType.type)
                {
                case USB::BmRequestType::Standard:
                    switch (request->bRequest)
                    {
                    case USB::RequestCode::ClearFeature:
                        if (request->wLength || request->wValue) break;
                        ums->listen();
                        size = 0;
                        break;
                    default: break;
                    }
                    break;
                default: break;
                }
                return size;
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
                ums->handleOutComplete(bytesLeft);
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }

            UMSAltSetting* ums;

        public:
            constexpr UMSOutEndpoint(UMSAltSetting* ums, int number)
                : Endpoint(USB::EndpointNumber(USB::Out, number)), ums(ums) {}
        };

        class __attribute__((aligned(4))) UMSInEndpoint : public USB::Endpoint
        {
            int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
            {
                return -1;
            }

            void xferComplete(USB::USB* usb, int bytesLeft)
            {
                ums->handleInComplete(bytesLeft);
            }

            void setupReceived(USB::USB* usb, uint32_t offset)
            {
            }

            UMSAltSetting* ums;

        public:
            constexpr UMSInEndpoint(UMSAltSetting* ums, int number)
                : Endpoint(USB::EndpointNumber(USB::In, number)), ums(ums) {}
        };

        const USB::Endpoint* const endpoints[2];
        UMSOutEndpoint outEp;
        UMSInEndpoint inEp;
        USB::USB* usb;

        struct
        {
            uint32_t sector;
            uint32_t count;
            uint32_t origCount;
            uint32_t curCmd;
            uint32_t tag;
            uint32_t lun;
            uint32_t lastResult;
            uint32_t bytesPending;
            bool dataDirection;
        } curCmd;

        struct
        {
            SCSI::SenseKey senseKey : 8;
            uint8_t information;
            SCSI::AdditionalSenseCode asc : 8;
            SCSI::AdditionalSenseCodeQualifier ascq : 8;
        } curSense;

        struct __attribute__((packed,aligned(4))) CommandBlockWrapper
        {
            uint32_t signature;
            uint32_t tag;
            uint32_t dataTransferLength;
            uint8_t flags;
            uint8_t lun;
            uint8_t commandLength;
            uint8_t commandBlock[16];
        };

        struct __attribute__((packed,aligned(4))) CommandStatusWrapper
        {
            uint32_t signature;
            uint32_t tag;
            uint32_t dataResidue;
            uint8_t status;
        };

        union __attribute__((packed,aligned(4)))
        {
            SCSI::InquiryData inquiry;
            SCSI::Capacity capacityData;
            SCSI::FormatCapacity formatCapacityData;
            SCSI::SenseDataFixed senseDataFixed;
            SCSI::ModeSenseData6 modeSense6;
            SCSI::ModeSenseData10 modeSense10;
            SCSI::ReportLunData lunData;
            CommandStatusWrapper csw;
            uint8_t data[512];
        } tb;

        CommandBlockWrapper cbw;
        Storage* storage;

    public:
        bool connected;
        bool ejected;
        bool locked;

    private:
        enum AsyncPending
        {
            Idle = 0,
            Read,
            Write,
        } asyncPending : 8;

        enum State
        {
            WaitingForCommand = 0,
            SendingBlocks,
            SendingResult,
            ReceivingBlocks,
            WaitingForCSWCompletion,
            Processing,
        } state;

        void listen()
        {
            state = WaitingForCommand;
            usb->startRx(outEp.number, &cbw, 31);
        }

        void fail(int reason)
        {
            usb->setStall(inEp.number, true);
            usb->setStall(outEp.number, true);
        }

        void sendCSW(int status)
        {
            if (curCmd.bytesPending)
            {
                if (curCmd.dataDirection) usb->setStall(inEp.number, true);
                else usb->setStall(outEp.number, true);
            }

            tb.csw.signature = 0x53425355;
            tb.csw.tag = curCmd.tag;
            tb.csw.dataResidue = curCmd.bytesPending;
            tb.csw.status = status;

            state = WaitingForCSWCompletion;
            usb->startTx(inEp.number, &tb.csw, 13);

            if (!status)
            {
                curSense.senseKey = (SCSI::SenseKey)0;
                curSense.information = 0;
                curSense.asc = (SCSI::AdditionalSenseCode)0;
                curSense.ascq =(SCSI::AdditionalSenseCodeQualifier) 0;
            }
        }

        void receiveBlockData(void* data, uint32_t size)
        {
            if (curCmd.dataDirection) fail(1);  // Attempting to receive data for IN command
            else if (size > curCmd.bytesPending) fail(2);  // Receive overrun
            else
            {
                curCmd.bytesPending -= size;
                usb->startRx(outEp.number, data, size);
                state = ReceivingBlocks;
            }
        }

        void sendBlockData(void* data, uint32_t size)
        {
            if (!curCmd.dataDirection) fail(3);  // Attempting to send data for OUT command
            else if (size > curCmd.bytesPending) fail(4);  // Send block overrun
            else
            {
                curCmd.bytesPending -= size;
                usb->startTx(inEp.number, data, size);
                state = SendingBlocks;
            }
        }

        void sendCommandResult(void* data, uint32_t size)
        {
            if (!curCmd.dataDirection) fail(5);  // Attempting to send result for OUT command!
            else if (size > curCmd.bytesPending) fail(6);  // Send result overrun
            else
            {
                curCmd.bytesPending -= size;
                usb->startTx(inEp.number, data, size);
                state = SendingResult;
            }
        }

        void sendCommandFailedResult(SCSI::SenseKey key, SCSI::AdditionalSenseCode asc,
                                     SCSI::AdditionalSenseCodeQualifier ascq)
        {
            sendCSW(1);
            curSense.senseKey = key;
            curSense.asc = asc;
            curSense.ascq = ascq;
        }

        void handleSCSI(CommandBlockWrapper* cbw)
        {
            uint32_t length = cbw->dataTransferLength;

            if (cbw->signature != 0x43425355)
            {
                usb->setStall(inEp.number, true);
                usb->setStall(outEp.number, true);
                return;
            }
            curCmd.tag = cbw->tag;
            curCmd.lun = cbw->lun;
            curCmd.curCmd = cbw->commandBlock[0];
            curCmd.bytesPending = cbw->dataTransferLength;
            if (cbw->flags & 0x80) curCmd.dataDirection = 1;
            else curCmd.dataDirection = 0;

            switch (cbw->commandBlock[0])
            {
                case SCSI::TestUnitReady:
                    if (!ejected) sendCSW(0);
                    else sendCommandFailedResult(SCSI::NotReady, SCSI::MediumNotPresent, SCSI::None);
                    break;

                case SCSI::ReportLUNs:
                {
                    memset(&tb.lunData, 0, sizeof(tb.lunData));
                    tb.lunData.lunListLength = 0x08000000;
                    sendCommandResult(&tb.lunData, MIN(16, length));
                    break;
                }

                case SCSI::Inquiry:
                    memset(&tb.inquiry, 0, sizeof(tb.inquiry));
                    memset(tb.inquiry.VendorId, ' ', sizeof(tb.inquiry.VendorId));
                    memset(tb.inquiry.ProductId, ' ', sizeof(tb.inquiry.ProductId));
                    memset(tb.inquiry.ProductRevisionLevel, ' ', sizeof(tb.inquiry.ProductRevisionLevel));
                    memcpy(tb.inquiry.ProductId, "microSD card", 12);
                    tb.inquiry.deviceType = SCSI::DirectAccessDevice;
                    tb.inquiry.additionalLength = 0x1f;
                    tb.inquiry.versions = 4;
                    tb.inquiry.format = 2;
                    tb.inquiry.deviceTypeModifier = SCSI::DeviceRemovable;
                    length = MIN(length, cbw->commandBlock[4]);
                    sendCommandResult(&tb.inquiry, MIN(sizeof(tb.inquiry), length));
                    break;

                case SCSI::RequestSense:
                {
                    tb.senseDataFixed.responseCode = 0x70;
                    tb.senseDataFixed.obsolete = 0;
                    tb.senseDataFixed.feiSenseKey = curSense.senseKey & 0x0f;
                    tb.senseDataFixed.information = curSense.information;
                    tb.senseDataFixed.additionalSenseLength = 10;
                    tb.senseDataFixed.commandSpecificInformation = 0;
                    tb.senseDataFixed.additionalSenseCode = curSense.asc;
                    tb.senseDataFixed.additionalSenseCodeQualifier = curSense.ascq;
                    tb.senseDataFixed.fieldReplaceableUnitCode = 0;
                    tb.senseDataFixed.sksv = 0;
                    tb.senseDataFixed.senseKeySpecific = 0;
                    sendCommandResult(&tb.senseDataFixed, MIN(sizeof(tb.senseDataFixed), length));
                    break;
                }

                case SCSI::ModeSense10:
                {
                    if (ejected)
                    {
                        sendCommandFailedResult(SCSI::NotReady, SCSI::MediumNotPresent, SCSI::None);
                        break;
                    }
                    tb.modeSense10.modeDataLength = htobe16(sizeof(tb.modeSense10) - 2);
                    tb.modeSense10.mediumType = 0;
                    tb.modeSense10.deviceSpecific = 0;
                    tb.modeSense10.reserved = 0;
                    tb.modeSense10.longLba = 1;
                    tb.modeSense10.blockDescriptorLength = 0;
                    sendCommandResult(&tb.modeSense10, MIN(sizeof(tb.modeSense10), length));
                    break;
                }

                case SCSI::ModeSense6:
                {
                    if (ejected)
                    {
                        sendCommandFailedResult(SCSI::NotReady, SCSI::MediumNotPresent, SCSI::None);
                        break;
                    }
                    tb.modeSense6.modeDataLength = sizeof(tb.modeSense6) - 1;
                    tb.modeSense6.mediumType = 0;
                    tb.modeSense6.deviceSpecific = 0;
                    tb.modeSense6.blockDescriptorLength = 0;
                    sendCommandResult(&tb.modeSense6, MIN(sizeof(tb.modeSense6), length));
                    break;
                }

                case SCSI::StartStopUnit:
                    if ((cbw->commandBlock[4] & 0xf3) == 2) ejected = true;
                    sendCSW(0);
                    break;

                case SCSI::AllowMediumRemoval:
                    if ((cbw->commandBlock[4] & 0x03) == 0) locked = false;
                    else locked = true;
                    sendCSW(0);
                    break;

                case SCSI::ReadFormatCapacity:
                {
                    if (!ejected)
                    {
                        tb.formatCapacityData.followingLength = 0x08000000;
                        tb.formatCapacityData.blockCount = swap32(storage->pageCount - 1);
                        tb.formatCapacityData.blockSize = swap32(512);
                        tb.formatCapacityData.blockSize |= swap32(0x02000000);
                        sendCommandResult(&tb.formatCapacityData, MIN(sizeof(tb.formatCapacityData), length));
                   }
                   else sendCommandFailedResult(SCSI::NotReady, SCSI::MediumNotPresent, SCSI::None);
                   break;
                }

                case SCSI::ReadCapacity:
                {
                    if (!ejected)
                    {
                        tb.capacityData.blockCount = swap32(storage->pageCount - 1);
                        tb.capacityData.blockSize = swap32(512);
                        sendCommandResult(&tb.capacityData, MIN(sizeof(tb.capacityData), length));
                    }
                    else sendCommandFailedResult(SCSI::NotReady, SCSI::MediumNotPresent, SCSI::None);
                    break;
                }

                case SCSI::Read10:
                    if (ejected)
                    {
                        sendCommandFailedResult(SCSI::NotReady, SCSI::MediumNotPresent, SCSI::None);
                        break;
                    }
                    curCmd.sector = (cbw->commandBlock[2] << 24 | cbw->commandBlock[3] << 16
                                    | cbw->commandBlock[4] << 8 | cbw->commandBlock[5]);
                    curCmd.count = (cbw->commandBlock[7] << 8 | cbw->commandBlock[8]);
                    curCmd.origCount = curCmd.count;

                    if ((curCmd.sector + curCmd.count) > storage->pageCount)
                        sendCommandFailedResult(SCSI::IllegalRequest, SCSI::LBAOutOfRange, SCSI::None);
                    else if (!curCmd.count) sendCSW(0);
                    else asyncPending = Read;
                    break;

                case SCSI::Write10:
                    if (ejected)
                    {
                        sendCommandFailedResult(SCSI::NotReady, SCSI::MediumNotPresent, SCSI::None);
                        break;
                    }
                    curCmd.sector = (cbw->commandBlock[2] << 24 | cbw->commandBlock[3] << 16
                                    | cbw->commandBlock[4] << 8 | cbw->commandBlock[5]);
                    curCmd.count = (cbw->commandBlock[7] << 8 | cbw->commandBlock[8]);
                    curCmd.origCount = curCmd.count;

                    if ((curCmd.sector + curCmd.count) > storage->pageCount)
                        sendCommandFailedResult(SCSI::IllegalRequest, SCSI::LBAOutOfRange, SCSI::None);
                    else if (!curCmd.count) sendCSW(0);
                    else receiveBlockData(&tb, 512);
                    break;

                case SCSI::WriteBuffer:
                    break;

                default:
                    sendCommandFailedResult(SCSI::IllegalRequest, SCSI::InvalidCommand, SCSI::None);
                    break;
            }
        }

        void handleOutComplete(int bytesLeft)
        {
            if (bytesLeft) fail(7);  // Short OUT transfer
            else
                switch (state)
                {
                    case ReceivingBlocks:
                        asyncPending = Write;
                        break;
                    case WaitingForCommand:
                        state = Processing;
                        handleSCSI(&cbw);
                        break;
                    default:
                        fail(8);  // Got OUT completion in invalid state
                }
        }

        void handleInComplete(int bytesLeft)
        {
            if (bytesLeft) fail(9);  // Short IN transfer
            else
                switch (state)
                {
                    case WaitingForCSWCompletion:
                        state = WaitingForCommand;
                        listen();
                        break;
                    case SendingResult:
                        sendCSW(0);
                        break;
                    case SendingBlocks:
                        if (!curCmd.count) sendCSW(0);
                        else asyncPending = Read;
                        break;
                    default:
                        fail(10);  // Got IN completion in invalid state
                }
        }

        int ctrlRequest(USB::USB* usb, USB::SetupPacket* request, const void** response)
        {
            int size = -1;
            switch (request->bmRequestType.type)
            {
            case USB::BmRequestType::Class:
                switch ((int)request->bRequest)
                {
                case 0xfe:  // GET_MAX_LUN
                    usb->buffer->u8[0] = 0;
                    size = 1;
                    break;
                case 0xff:  // STORAGE_RESET
                    size = 0;
                    break;
                default: break;
                }
                break;
            default: break;
            }
            return size;
        }

        void set(USB::USB* usb)
        {
            this->usb = usb;
            ejected = false;
            locked = false;
            connected = true;
            usb->configureEp(outEp.number, USB::Bulk, 64);
            usb->configureEp(inEp.number, USB::Bulk, 64);
            listen();
        }

        void unset(USB::USB* usb)
        {
            usb->unconfigureEp(outEp.number);
            usb->unconfigureEp(inEp.number);
            connected = false;
        }

    public:
        bool isConnected()
        {
            return connected && !ejected;
        }

        bool handleAsync()
        {
            if (!isConnected()) return false;
            switch (asyncPending)
            {
            case Idle:
                break;
            case Read:
                if (storage->read(curCmd.sector, 1, &tb) != Storage::RESULT_OK)
                    sendCommandFailedResult(SCSI::MediumError, SCSI::ReadError, SCSI::None);
                else
                {
                    sendBlockData(&tb, 512);
                    curCmd.sector++;
                    curCmd.count--;
                }
                break;
            case Write:
                if (storage->write(curCmd.sector, 1, &tb) != Storage::RESULT_OK)
                    sendCommandFailedResult(SCSI::MediumError, SCSI::WriteError, SCSI::None);
                else
                {
                    curCmd.sector++;
                    if (--curCmd.count) receiveBlockData(&tb, 512);
                    else sendCSW(0);
                }
                break;
            }
            asyncPending = Idle;
            return true;
        }

        constexpr UMSAltSetting(Storage* storage, int outEpNum, int inEpNum)
            : AltSetting(ARRAYLEN(endpoints)), endpoints{&outEp, &inEp}, outEp(this, outEpNum), inEp(this, inEpNum),
              usb(NULL), curCmd{}, curSense{}, tb{}, cbw{}, storage(storage), connected(false),
              ejected(false), locked(false), asyncPending(Idle), state(WaitingForCommand) {}
    };

    const USB::AltSetting* const altSettings[1];
    UMSAltSetting altSetting;

public:
    constexpr UMSInterface(Storage* storage, int outEpNum, int inEpNum)
        : Interface(ARRAYLEN(altSettings)), altSettings{&altSetting}, altSetting(storage, outEpNum, inEpNum) {}
};

class __attribute__((aligned(4))) UMSConfiguration : public USB::Configuration
{
    static constexpr const struct __attribute__((packed)) _config_descriptor
    {
        USB::Descriptor::ConfigurationDescriptor cfg;
        USB::Descriptor::InterfaceDescriptor i0_a0;
        USB::Descriptor::EndpointDescriptor i0_a0_eps[2];
    } descriptor
    {
        USB::Descriptor::ConfigurationDescriptor(sizeof(struct _config_descriptor), 1, 1, 0,
                                                 USB::Descriptor::ConfigAttributes(true, true, false), 100),
        USB::Descriptor::InterfaceDescriptor(0, 0, ARRAYLEN(_config_descriptor::i0_a0_eps),
                                             USB::Descriptor::Class(0x08, 0x06, 0x50), 0),
        {
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::Out, 1),
                                                USB::EndpointAttributes(USB::Bulk, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 0),
            USB::Descriptor::EndpointDescriptor(USB::EndpointNumber(USB::In, 1),
                                                USB::EndpointAttributes(USB::Bulk, USB::EndpointAttributes::NoSync,
                                                                        USB::EndpointAttributes::Data), 64, 0),
        },
    };

public:
    const USB::Interface* const interfaces[1];
    UMSInterface interface;

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
    constexpr UMSConfiguration(Storage* storage)
        : Configuration(&descriptor.cfg, ARRAYLEN(interfaces)), interfaces{&interface}, interface(storage, 1, 1) {}
};
constexpr const UMSConfiguration::_config_descriptor UMSConfiguration::descriptor;

static UMSConfiguration umsConfig(&sd);

static USB::Configuration* const usbConfigs[] =
{
    &umsConfig,
};

static const USB::Descriptor::DeviceDescriptor usbDevDesc(0x200, USB::Descriptor::Class(0, 0, 0), 64,
                                                          0xffff, 0x4453, 0x0100, 1, 2, 3, ARRAYLEN(usbConfigs));

static const USB_DECLARE_STRDESC(u"\x0409") usbStrLanguage;
static const USB_DECLARE_STRDESC(u"Michael Sparmann") usbStrVendor;
static const USB_DECLARE_STRDESC(u"SD Card Reader") usbStrProduct;
static const USB_DECLARE_STRDESC(u"") usbStrSerial;
static const USB::Descriptor::StringDescriptor* const usbStrDescs[] =
{
    &usbStrLanguage.header,
    &usbStrVendor.header,
    &usbStrProduct.header,
    &usbStrSerial.header,
};

static STM32::USB usb(&usbDevDesc, usbStrDescs, ARRAYLEN(usbStrDescs), usbConfigs, ARRAYLEN(usbConfigs));


void usb_init()
{
    usb.start();
}

bool usb_poll()
{
    if (!umsConfig.interface.altSetting.isConnected()) return false;
    lcd.print(0, 0, lcd.defaultFont, 0, "USB mode          ");
    while (umsConfig.interface.altSetting.handleAsync()) idle();
    return true;
}

bool usb_busy()
{
    return umsConfig.interface.altSetting.isConnected();
}
