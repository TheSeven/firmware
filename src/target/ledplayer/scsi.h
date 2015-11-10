#pragma once
#include "global.h"


namespace SCSI
{
    enum Command
    {
        TestUnitReady = 0x00,
        Inquiry = 0x12,
        ModeSense6 = 0x1a,
        ModeSense10 = 0x5a,
        RequestSense = 0x03,
        AllowMediumRemoval = 0x1e,
        ReadCapacity = 0x25,
        ReadFormatCapacity = 0x23,
        Read10 = 0x28,
        Write10 = 0x2a,
        StartStopUnit = 0x1b,
        ReportLUNs = 0xa0,
        WriteBuffer = 0x3b,
    };

    enum SenseKey
    {
        SoftError = 0x01,
        NotReady = 0x02,
        MediumError = 0x03,
        HardwareError = 0x04,
        IllegalRequest = 0x05,
        UnitAttention = 0x06,
        DataProtect = 0x07,
        AbortedCommand = 0x0b,
    };

    enum AdditionalSenseCode
    {
        MediumNotPresent = 0x3a,
        InvalidFieldInCBD = 0x24,
        LBAOutOfRange = 0x21,
        WriteError = 0x0c,
        ReadError = 0x11,
        ASCNotReady = 0x04,
        InvalidCommand = 0x20,
        InternalTargetFailure = 0x44,
        AddressMarkNotFound = 0x13,
        OperatorRequest = 0x5a,
        MediumMayHaveChanged = 0x28,
        SCSIParityError = 0x47,
        WriteProtected = 0x27,
        RecoveredError = 0x00,
        UnsuccessfulSoftReset = 0x46,
    };

    enum AdditionalSenseCodeQualifier
    {
        None = 0x00,
        BecomingReady = 0x01,
        MediumRemovalRequest = 0x01,
        UICRCErrorDetected = 0x03,
    };

    enum DeviceType
    {
        DirectAccessDevice = 0x00,
    };

    enum DeviceTypeModifier
    {
        DeviceRemovable = 0x80,
    };

    struct __attribute__((packed)) InquiryData
    {
        uint8_t deviceType;
        uint8_t deviceTypeModifier;
        uint8_t versions;
        uint8_t format;
        uint8_t additionalLength;
        uint16_t : 16;
        uint8_t Capability;
        char VendorId[8];
        char ProductId[16];
        char ProductRevisionLevel[4];
    };

    struct __attribute__((packed)) ReportLunData
    {
        uint32_t lunListLength;
        uint32_t : 32;
        uint8_t luns[1][8];
    };

    struct __attribute__((packed)) SenseDataFixed
    {
        uint8_t responseCode;
        uint8_t obsolete;
        uint8_t feiSenseKey;
        uint32_t information;
        uint8_t additionalSenseLength;
        uint32_t  commandSpecificInformation;
        uint8_t additionalSenseCode;
        uint8_t additionalSenseCodeQualifier;
        uint8_t fieldReplaceableUnitCode;
        uint8_t sksv;
        uint16_t senseKeySpecific;
    };

    struct __attribute__((packed)) ModeSenseData10
    {
        uint16_t modeDataLength;
        uint8_t mediumType;
        uint8_t deviceSpecific;
        uint8_t longLba;
        uint8_t reserved;
        uint16_t blockDescriptorLength;
    };

    struct __attribute__((packed)) ModeSenseData6
    {
        uint8_t modeDataLength;
        uint8_t mediumType;
        uint8_t deviceSpecific;
        uint8_t blockDescriptorLength;
    };

    struct __attribute__((packed)) Capacity
    {
        uint32_t blockCount;
        uint32_t blockSize;
    };

    struct __attribute__((packed)) FormatCapacity
    {
        uint32_t followingLength;
        uint32_t blockCount;
        uint32_t blockSize;
    };
}
