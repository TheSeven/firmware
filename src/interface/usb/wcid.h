#pragma once

#include "global.h"
#include "interface/usb/usb.h"


#define USB_WCID_CAPABILITY_UUID ::USB::Descriptor::Capability::UUID({0xd8dd60df, 0x4589, 0x4cc7, {0x9c, 0xd2}, \
                                                                      {0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f}})

#define USB_WICD_DECLARE_REGKEY(type, name, data) \
    struct \
    { \
        ::USB::WCID::Descriptor::RegProperty header = ::USB::WCID::Descriptor::RegProperty(type); \
        uint16_t wPropertyNameLength = sizeof(name); \
        char PropertyName[sizeof(name)] = name; \
        uint16_t wPropertyDataLength = sizeof(data); \
        uint8_t PropertyData[sizeof(data)] = data; \
    }


namespace USB
{
    namespace WCID
    {
        struct __attribute__((packed)) DescriptorSetInfo
        {
            uint32_t dwWindowsVersion;
            uint16_t wMSOSDescriptorSetTotalLength;
            uint8_t bMSVendorCode;
            uint8_t bAltEnumCode;
            constexpr DescriptorSetInfo(uint32_t windowsVersion, uint16_t descriptorSetTotalLen,
                                        uint8_t vendorCode, uint8_t altEnumCode)
                : dwWindowsVersion(windowsVersion), wMSOSDescriptorSetTotalLength(descriptorSetTotalLen),
                  bMSVendorCode(vendorCode), bAltEnumCode(altEnumCode) {}

        };

        namespace Descriptor
        {
            enum __attribute__((packed)) Type
            {
                Type_SetHeader = 0,
                Type_ConfigurationSubsetHeader = 1,
                Type_FunctionSubsetHeader = 2,
                Type_CompatibleID = 3,
                Type_RegProperty = 4,
                Type_MinResumeTime = 5,
                Type_ModelID = 6,
                Type_CCGPDevice = 7,
            };

            struct __attribute__((packed)) Header
            {
                uint16_t wLength;
                Type wDescriptorType : 16;
                constexpr Header(uint16_t len, Type type) : wLength(len), wDescriptorType(type) {}
            };

            struct __attribute__((packed)) SetHeader : public Header
            {
                uint32_t dwWindowsVersion;
                uint16_t wTotalLength;
                constexpr SetHeader(uint32_t windowsVersion, uint16_t totalLen)
                    :  Header(sizeof(SetHeader), Type_SetHeader),
                       dwWindowsVersion(windowsVersion), wTotalLength(totalLen) {}
            };

            struct __attribute__((packed)) ConfigurationSubsetHeader : public Header
            {
                uint8_t bConfigurationValue;
                uint8_t bReserved;
                uint16_t wTotalLength;
                constexpr ConfigurationSubsetHeader(uint8_t configValue, uint16_t totalLen)
                    :  Header(sizeof(ConfigurationSubsetHeader), Type_ConfigurationSubsetHeader),
                       bConfigurationValue(configValue), bReserved(0), wTotalLength(totalLen) {}
            };

            struct __attribute__((packed)) FunctionSubsetHeader : public Header
            {
                uint8_t bFirstInterface;
                uint8_t bReserved;
                uint16_t wTotalLength;
                constexpr FunctionSubsetHeader(uint8_t firstInterface, uint16_t totalLen)
                    :  Header(sizeof(FunctionSubsetHeader), Type_FunctionSubsetHeader),
                       bFirstInterface(firstInterface), bReserved(0), wTotalLength(totalLen) {}
            };

            struct __attribute__((packed)) CompatibleID : public Header
            {
                uint64_t compatibleID;
                uint64_t subCompatibleID;
                constexpr CompatibleID(uint64_t compatibleID, uint64_t subCompatibleID)
                    : Header(sizeof(CompatibleID), Type_CompatibleID),
                      compatibleID(compatibleID), subCompatibleID(subCompatibleID) {}
            };

            struct __attribute__((packed)) RegProperty : public Header
            {
                enum Type
                {
                    REG_SZ = 1,
                    REG_EXPAND_SZ = 2,
                    REG_BINARY = 3,
                    REG_DWORD_LITTLE_ENDIAN = 4,
                    REG_DWORD_BIG_ENDIAN = 5,
                    REG_LINK = 6,
                    REG_MULTI_SZ = 7,
                } wPropertyDataType : 16;
                constexpr RegProperty(Type type)
                    : Header(sizeof(RegProperty), Type_RegProperty), wPropertyDataType(type) {}
            };

            struct __attribute__((packed)) MinResumeTime : public Header
            {
                uint8_t bResumeRecoveryTime;
                uint8_t bResumeSignalingTime;
                constexpr MinResumeTime(uint8_t recoveryTime, uint8_t signalingTime)
                    : Header(sizeof(MinResumeTime), Type_MinResumeTime),
                      bResumeRecoveryTime(recoveryTime), bResumeSignalingTime(signalingTime) {}
            };

            struct __attribute__((packed)) ModelID : public Header
            {
                ::USB::Descriptor::Capability::UUID modelID;
                constexpr ModelID(::USB::Descriptor::Capability::UUID modelID)
                    : Header(sizeof(ModelID), Type_ModelID), modelID(modelID) {}
            };

            struct __attribute__((packed)) CCGPDevice : public Header
            {
                constexpr CCGPDevice() : Header(sizeof(CCGPDevice), Type_CCGPDevice) {}
            };
        }

        enum __attribute__((packed)) RequestCode
        {
            GetDescriptorSet = 7,
            SetAltEnumeration = 8,
        };

        extern int handleGetDescriptorSet(USB* usb, SetupPacket* request, const void** response,
                                          const Descriptor::SetHeader* wcidSet);

    }
}
