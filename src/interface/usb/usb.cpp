#include "global.h"
#include "interface/usb/usb.h"
#include "sys/util.h"


void USB::USB::unconfigure()
{
    // Notify a configuration and its active interface altsettings that it was just kicked out.
    if (!currentConfiguration) return;
    Configuration* configuration = configurations[currentConfiguration - 1];
    for (int i = 0; i < configuration->interfaceCount; i++)
    {
        Interface* interface = configuration->interfaces[i];
        AltSetting* altSetting = interface->altSettings[interface->currentAltSetting];
        altSetting->unset(this);
    }
    configuration->unset(this);
    currentConfiguration = 0;
}

USB::Endpoint* USB::USB::findEndpoint(EndpointNumber ep, int* ifIdx, int* epIdx)
{
    // Figure out who's currently owning a (hardware) endpoint.
    // Returns a pointer to the Endpoint object, and sets *epIdx to its index within its altsetting.
    // *ifIdx will be set to the interface number within the current configuration that the altsetting belongs to.
    // If nobody currently owns the specified endpoint, return NULL (*epIdx and *ifIdx will contain garbage).
    if (!currentConfiguration) return NULL;
    Configuration* configuration = configurations[currentConfiguration - 1];
    for (*ifIdx = 0; *ifIdx < configuration->interfaceCount; (*ifIdx)++)
    {
        Interface* interface = configuration->interfaces[*ifIdx];
        AltSetting* altSetting = interface->altSettings[interface->currentAltSetting];
        for (*epIdx = 0; *epIdx < altSetting->endpointCount; (*epIdx)++)
        {
            Endpoint* endpoint = altSetting->endpoints[*epIdx];
            if (endpoint->number.u8 == ep.u8) return endpoint;
        }
    }
    return NULL;
}

void USB::USB::handleEp0Setup(SetupPacket* packet)
{
    // size < -2: do nothing at all (dangerous, you need to take care of priming EP0 yourself!)
    //            (this case is required for requests that have an OUT data stage)
    // size == -2: send STALL
    // size == -1: try to run default handler, or send STALL if none exists
    // size == 0: send ACK
    // size > 0: send <size> bytes at <addr>, then expect ACK
    const void* addr = buffer;
    int size = -1;
    switch (packet->bmRequestType.recipient)
    {
    case BmRequestType::Device:
        if (ctrlRequestHook) size = ctrlRequestHook(this, packet, &addr);
        if (size != -1) break;
        switch (packet->bmRequestType.type)
        {
        case BmRequestType::Standard:
            switch (packet->bRequest)
            {
            case GetStatus:
                if (packet->wLength != 2 || packet->wIndex || !currentAddress || packet->wValue) break;
                buffer->u8[0] = 0;
                buffer->u8[1] = 1;
                size = 2;
                break;
            case SetAddress:
                if (packet->wLength || packet->wIndex || packet->wValue > 127) break;
                currentAddress = packet->wValue;
                // We can already set the address here, the driver has to take care to send the ACK with the old address.
                drvSetAddress(currentAddress);
                size = 0;
                break;
            case GetDescriptor:
                if (!packet->wLength) break;
                switch (packet->wValue >> 8)
                {
                case Descriptor::Device:
                    if ((packet->wValue & 0xff) || packet->wIndex) break;
                    addr = deviceDescriptor;
                    size = deviceDescriptor->bLength;
                    break;
                case Descriptor::BOS:
                    if ((packet->wValue & 0xff) || packet->wIndex || !bosDescriptor) break;
                    addr = bosDescriptor;
                    size = bosDescriptor->wTotalLength;
                    break;
                case Descriptor::Configuration:
                    if (packet->wIndex
                     || (packet->wValue & 0xff) >= configurationCount) break;
                    addr = configurations[packet->wValue & 0xff]->descriptor;
                    size = configurations[packet->wValue & 0xff]->descriptor->wTotalLength;
                    break;
                case Descriptor::String:
                    if ((packet->wValue & 0xff) > stringDescriptorCount) break;
                    addr = stringDescriptors[packet->wValue & 0xff];
                    size = stringDescriptors[packet->wValue & 0xff]->bLength;
                    break;
                default: break;
                }
                if (size > packet->wLength) size = packet->wLength;
                break;
            case GetConfiguration:
                if (packet->wLength != 1 || packet->wIndex || !currentAddress || packet->wValue) break;
                buffer->u8[0] = currentConfiguration;
                size = 1;
                break;
            case SetConfiguration:
                if (packet->wLength || packet->wIndex || !currentAddress || packet->wValue > configurationCount) break;
                unconfigure();
                currentConfiguration = packet->wValue;
                if (currentConfiguration)
                {
                    // Notify the configuration and its interface default altsettings that it should set up stuff
                    Configuration* configuration = configurations[currentConfiguration - 1];
                    configuration->set(this);
                    int i;
                    for (i = 0; i < configuration->interfaceCount; i++)
                    {
                        Interface* interface = configuration->interfaces[i];
                        interface->currentAltSetting = 0;
                        AltSetting* altSetting = interface->altSettings[0];
                        altSetting->set(this);
                    }
                }
                size = 0;
                break;
            default: break;
            }
            break;
        default: break;
        }
        break;
    case BmRequestType::Interface:
    {
        if (!currentConfiguration) break;
        Configuration* configuration = configurations[currentConfiguration - 1];
        int intfId = packet->wIndex;
        if (intfId >= configuration->interfaceCount) break;
        Interface* interface = configuration->interfaces[intfId];
        size = interface->altSettings[interface->currentAltSetting]->ctrlRequest(this, packet, &addr);
        if (size != -1) break;
        switch (packet->bmRequestType.type)
        {
        case BmRequestType::Standard:
            switch (packet->bRequest)
            {
            case GetStatus:
                if (packet->wLength != 2 || packet->wValue) break;
                buffer->u8[0] = 0;
                buffer->u8[1] = 0;
                size = 2;
                break;
            case GetInterface:
                if (packet->wLength != 1 || packet->wValue) break;
                buffer->u8[0] = interface->currentAltSetting;
                size = 1;
                break;
            case SetInterface:
            {
                if (packet->wLength || packet->wValue > interface->altSettingCount) break;
                interface->altSettings[interface->currentAltSetting]->unset(this);
                interface->currentAltSetting = packet->wValue;
                interface->altSettings[interface->currentAltSetting]->set(this);
                break;
            }
            default: break;
            }
            break;
        default: break;
        }
        break;
    }
    case BmRequestType::Endpoint:
    {
        if (!currentConfiguration) break;
        EndpointNumber ep(packet->wIndex);
        int intfId;
        int epId;
        Endpoint* endpoint = findEndpoint(ep, &intfId, &epId);
        if (!endpoint) break;
        size = endpoint->ctrlRequest(this, packet, &addr);
        if (size != -1) break;
        switch (packet->bmRequestType.type)
        {
        case BmRequestType::Standard:
            switch (packet->bRequest)
            {
            case GetStatus:
                if (packet->wLength != 2 || packet->wValue) break;
                buffer->u8[0] = 0;
                buffer->u8[1] = drvGetStall(ep);
                size = 2;
                break;
            case ClearFeature:
                if (packet->wLength || packet->wValue) break;
                setStall(ep, false);
                size = 0;
                break;
            case SetFeature:
                if (packet->wLength || packet->wValue) break;
                setStall(ep, true);
                size = 0;
                break;
            default: break;
            }
            break;
            default: break;
        }
        break;
    }
    default: break;
    }
    // See comment at the top of this function for meanings of size.
    if (size == 0)
    {
        // Send ACK. Stall OUT pipe. Accept SETUP packets though.
        setStall(EndpointNumber(Out, 0), true);
        ep0StartRx(false, 0, NULL);
        ep0StartTx(NULL, 0, &ep0AckCallback);
    }
    else if (size > 0)
    {
        // Send a data stage. Expect to receive only SETUP until we're done. NAK everything else.
        ep0StartRx(false, 0, NULL);
        ep0TxPtr = addr;
        ep0TxLen = size;
        ep0FullTxCallback(this, EndpointNumber(In, 0), 0);  // A convenient way to start a transfer.
    }
    else if (size >= -2)
    {
        // We have no handler, or the handler failed. STALL everything, accept only SETUP packets.
        setStall(EndpointNumber(In, 0), true);
        setStall(EndpointNumber(Out, 0), true);
        ep0StartRx(false, 0, NULL);
    }
}

void USB::USB::handleBusReset(int highSpeed)
{
    currentAddress = 0;
    unconfigure();
    this->highSpeed = highSpeed;
    if (busResetHook) busResetHook(this);
    for (int c = 0; c < configurationCount; c++)
    {
        Configuration* configuration = configurations[c];
        configuration->busReset(this);
    }

    // Prime EP0 for the first setup packet.
    ep0StartRx(false, 0, NULL);
}

void USB::USB::handleXferComplete(EndpointNumber epNum, int bytesLeft)
{
    if (!epNum.number)
    {
        bool (*callback)(USB* data, EndpointNumber epNum, int size);
        if (epNum.direction == Out)
        {
            callback = ep0RxCallback;
            ep0RxCallback = NULL;
        }
        else
        {
            callback = ep0TxCallback;
            ep0TxCallback = NULL;
        }
        if (callback) callback(this, epNum, bytesLeft);
    }
    else
    {
        int epIdx;
        int ifIdx;
        Endpoint* endpoint = findEndpoint(epNum, &ifIdx, &epIdx);
        if (!endpoint) unconfigureEp(epNum);
        else endpoint->xferComplete(this, bytesLeft);
    }
}

void USB::USB::handleSetupReceived(EndpointNumber epNum, uint32_t offset)
{
    if (!epNum.number)
    {
        SetupPacket* packet = (SetupPacket*)&buffer->u8[offset];
        if (!ep0SetupHook || !ep0SetupHook(this, packet)) handleEp0Setup(packet);
    }
    else
    {
        int epIdx;
        int ifIdx;
        Endpoint* endpoint = findEndpoint(epNum, &ifIdx, &epIdx);
        if (!endpoint) unconfigureEp(epNum);
        else endpoint->setupReceived(this, offset);
    }
}

void USB::USB::start()
{
    // Initialize data structures
    currentAddress = 0;
    currentConfiguration = 0;

    // Initialize driver
    drvStart();
}

void USB::USB::stop()
{
    // Shut down driver
    drvStop();

    // Unconfigure everything
    unconfigure();
}

void USB::USB::ep0StartRx(bool non_setup, int len, bool (*callback)(USB* usb, EndpointNumber epNum, int bytesLeft))
{
    ep0RxCallback = callback;
    drvEp0StartRx(non_setup, len);
}

void USB::USB::ep0StartTx(const void* buf, int len, bool (*callback)(USB* usb, EndpointNumber epNum, int bytesLeft))
{
    if (needsAlign && ((uint32_t)buf) & (CACHEALIGN_SIZE - 1))
    {
        memcpy(buffer->u8, buf, len);
        buf = buffer->u8;
    }

    ep0TxCallback = callback;
    drvEp0StartTx(buf, len);
}

bool USB::USB::ep0FullTxCallback(USB* usb, EndpointNumber epNum, int bytesLeft)
{
    return usb->ep0FullTxCallback(epNum, bytesLeft);
}

bool USB::USB::ep0FullTxCallback(EndpointNumber epNum, int bytesLeft)
{
    if (bytesLeft || !ep0TxLen)
    {
        // This was the last packet. Expect zero-length ACK.
        ep0StartRx(true, 0, &ep0AckCallback);
        // If someone might expect to receive more data, send a zero-byte packet.
        if (!ep0TxLen) ep0StartTx(NULL, 0, &ep0ShortTxCallback);
        // Abuse this as a marker for ep0_short_tx_callback...
        ep0TxPtr = NULL;
        return false;
    }

    int len = MIN(64, ep0TxLen);
    ep0TxLen -= len;
    bool (*callback)(USB* data, EndpointNumber epNum, int size);
    if (len < 64) callback = &ep0ShortTxCallback;
    else callback = &ep0FullTxCallback;
    ep0StartTx(ep0TxPtr, len, callback);
    ep0TxPtr = (void*)(((int)ep0TxPtr) + 64);
    return true;
}

bool USB::USB::ep0ShortTxCallback(USB* usb, EndpointNumber epNum, int bytesLeft)
{
    return usb->ep0ShortTxCallback(epNum, bytesLeft);
}

bool USB::USB::ep0ShortTxCallback(EndpointNumber epNum, int bytesLeft)
{
    // No more data to be sent after a short packet. STALL IN.
    setStall(EndpointNumber(In, 0), true);
    // If this was the last regular packet, but a short one, expect zero-length ACK.
    // Otherwise ep0_tx_callback will have taken care of that already.
    if (ep0TxPtr) ep0StartRx(true, 0, &ep0AckCallback);
    return false;
}

bool USB::USB::ep0AckCallback(USB* usb, EndpointNumber epNum, int bytesLeft)
{
    return usb->ep0AckCallback(epNum, bytesLeft);
}

bool USB::USB::ep0AckCallback(EndpointNumber epNum, int bytesLeft)
{
    // This function is intended to eat zero-byte ACK packets,
    // which are always the last packet of a transaction.
    // At this point we can STALL all further packets.
    setStall(EndpointNumber(Out, 0), true);
    setStall(EndpointNumber(In, 0), true);
    // If this is an IN endpoint, something else must already be
    // waiting for a SETUP packet anyway, or there could be deadlocks.
    if (epNum.direction == Out) ep0StartRx(false, 0, NULL);
    return true;
}
