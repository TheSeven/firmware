#pragma once

struct __attribute__((packed)) STM32_ETH_REG_TYPE
{
    union MACCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 2;
            uint32_t RE : 1;
            uint32_t TE : 1;
            uint32_t DC : 1;
            enum BL
            {
                BL_10 = 0x0,
                BL_8 = 0x1,
                BL_4 = 0x2,
                BL_1 = 0x3,
            } BL : 2;
            uint32_t APCS : 1;
            uint32_t : 1;
            uint32_t RD : 1;
            uint32_t IPCO : 1;
            uint32_t DM : 1;
            uint32_t LM : 1;
            uint32_t ROD : 1;
            uint32_t FES : 1;
            uint32_t : 1;
            uint32_t CSD : 1;
            enum IFG
            {
                IFG_96Bit = 0x0,
                IFG_88Bit = 0x1,
                IFG_80Bit = 0x2,
                IFG_72Bit = 0x3,
                IFG_64Bit = 0x4,
                IFG_56Bit = 0x5,
                IFG_48Bit = 0x6,
                IFG_40Bit = 0x7,
            } IFG : 3;
            uint32_t : 2;
            uint32_t JD : 1;
            uint32_t WD : 1;
            uint32_t : 8;
        } b;
    } MACCR;
    union MACFFR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PM : 1;
            uint32_t HU : 1;
            uint32_t HM : 1;
            uint32_t DAIF : 1;
            uint32_t PAM : 1;
            uint32_t BFD : 1;
            enum PCF
            {
                PCF_BlockAll = 0x1,
                PCF_ForwardAll = 0x2,
                PCF_ForwardPassedAddrFilter = 0x3,
            } PCF : 2;
            uint32_t SAIF : 1;
            uint32_t SAF : 1;
            uint32_t HPF : 1;
            uint32_t : 20;
            uint32_t RA : 1;
        } b;
    } MACFFR;
    uint32_t MACHTHR;
    uint32_t MACHTLR;
    union MACMIIAR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MB : 1;
            uint32_t MW : 1;
            enum CR
            {
                CR_Div42 = 0x0,
                CR_Div62 = 0x1,
                CR_Div16 = 0x2,
                CR_Div26 = 0x3,
                CR_Div102 = 0x4,
            } CR : 3;
            uint32_t : 1;
            uint32_t MR : 5;
            uint32_t PA : 5;
            uint32_t : 16;
        } b;
    } MACMIIAR;
    union MACMIIDR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MD : 16;
            uint32_t : 16;
        } b;
    } MACMIIDR;
    union MACFCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t FCBBPA : 1;
            uint32_t TFCE : 1;
            uint32_t RFCE : 1;
            uint32_t UPFD : 1;
            enum PLT
            {
                PLT_Minus4 = 0x0,
                PLT_Minus28 = 0x1,
                PLT_Minus144 = 0x2,
                PLT_Minus256 = 0x3,
            } PLT : 2;
            uint32_t : 1;
            uint32_t ZQPD : 1;
            uint32_t : 8;
            uint32_t PT : 16;
        } b;
    } MACFCR;
    union MACVLANTR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t VLANTI : 16;
            uint32_t VLANTC : 1;
            uint32_t : 15;
        } b;
    } MACVLANTR;
    uint32_t RESERVED0[2];
    uint32_t MACRWUFFR;
    union MACPMTCSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t PD : 1;
            uint32_t MPE : 1;
            uint32_t WFE : 1;
            uint32_t : 2;
            uint32_t MPR : 1;
            uint32_t WFR : 1;
            uint32_t : 2;
            uint32_t GU : 1;
            uint32_t : 21;
            uint32_t WFFRPR : 1;
        } b;
    } MACPMTCSR;
    uint32_t RESERVED1;
    union MACDBGR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MMRPEA : 1;
            uint32_t MSFRWCS : 2;
            uint32_t : 1;
            uint32_t RFWRA : 1;
            enum RFRCS
            {
                RFRCS_IDLE = 0x0,
                RFRCS_READING_DATA = 0x1,
                RFRCS_READING_STATUS = 0x2,
                RFRCS_FLUSHING = 0x3,
            } RFRCS : 2;
            uint32_t : 1;
            enum RFFL
            {
                RFFL_EMPTY = 0x0,
                RFFL_BELOW = 0x1,
                RFFL_ABOVE = 0x2,
                RFFL_FULL = 0x3,
            } RFFL : 2;
            uint32_t : 6;
            uint32_t MMTEA : 1;
            enum MTFCS
            {
                MTFCS_IDLE = 0x0,
                MTFCS_WAITING = 0x1,
                MTFCS_TX_PAUSE = 0x2,
                MTFCS_TRANSFERRING = 0x3,
            } MTFCS : 2;
            uint32_t MTP : 1;
            enum TFRS
            {
                TFRS_IDLE = 0x0,
                TFRS_READING = 0x1,
                TFRS_WAITING = 0x2,
                TFRS_WRITING = 0x3,
            } TFRS : 2;
            uint32_t TFWA : 1;
            uint32_t : 1;
            uint32_t TFNE : 1;
            uint32_t TFF : 1;
            uint32_t : 6;
        } b;
    } MACDBGR;
    union MACSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 3;
            uint32_t PMTS : 1;
            uint32_t MMCS : 1;
            uint32_t MMMCRS : 1;
            uint32_t MMCTS : 1;
            uint32_t : 2;
            uint32_t TSTS : 1;
            uint32_t : 22;
        } b;
    } MACSR;
    union MACIMR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 3;
            uint32_t PMTIM : 1;
            uint32_t : 5;
            uint32_t TSTIM : 1;
            uint32_t : 22;
        } b;
    } MACIMR;
    union MACA0HR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MACA0H : 16;
            uint32_t : 16;
        } b;
    } MACA0HR;
    uint32_t MACA0LR;
    union MACA1HR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MACA1H : 16;
            uint32_t : 8;
            enum MBC
            {
                MBC_LBits7_0 = 0x1,
                MBC_LBits15_8 = 0x2,
                MBC_LBits23_16 = 0x4,
                MBC_LBits31_24 = 0x8,
                MBC_HBits7_0 = 0x10,
                MBC_HBits15_8 = 0x20,
            } MBC : 6;
            uint32_t SA : 1;
            uint32_t AE : 1;
        } b;
    } MACA1HR;
    uint32_t MACA1LR;
    union MACA2HR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MACA2H : 16;
            uint32_t : 8;
            enum MBC
            {
                MBC_LBits7_0 = 0x1,
                MBC_LBits15_8 = 0x2,
                MBC_LBits23_16 = 0x4,
                MBC_LBits31_24 = 0x8,
                MBC_HBits7_0 = 0x10,
                MBC_HBits15_8 = 0x20,
            } MBC : 6;
            uint32_t SA : 1;
            uint32_t AE : 1;
        } b;
    } MACA2HR;
    uint32_t MACA2LR;
    union MACA3HR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MACA3H : 16;
            uint32_t : 8;
            enum MBC
            {
                MBC_LBits7_0 = 0x1,
                MBC_LBits15_8 = 0x2,
                MBC_LBits23_16 = 0x4,
                MBC_LBits31_24 = 0x8,
                MBC_HBits7_0 = 0x10,
                MBC_HBits15_8 = 0x20,
            } MBC : 6;
            uint32_t SA : 1;
            uint32_t AE : 1;
        } b;
    } MACA3HR;
    uint32_t MACA3LR;
    uint32_t RESERVED2[40];
    union MMCCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t CR : 1;
            uint32_t CSR : 1;
            uint32_t ROR : 1;
            uint32_t MCF : 1;
            uint32_t MCP : 1;
            uint32_t MCFHP : 1;
            uint32_t : 26;
        } b;
    } MMCCR;
    union MMCRIR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 5;
            uint32_t RFCES : 1;
            uint32_t RFAES : 1;
            uint32_t : 10;
            uint32_t RGUFS : 1;
            uint32_t : 14;
        } b;
    } MMCRIR;
    union MMCTIR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 14;
            uint32_t TGFSCS : 1;
            uint32_t TGFMSCS : 1;
            uint32_t : 5;
            uint32_t TGFS : 1;
            uint32_t : 10;
        } b;
    } MMCTIR;
    union MMCRIMR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 5;
            uint32_t RFCEM : 1;
            uint32_t RFAEM : 1;
            uint32_t : 10;
            uint32_t RGUFM : 1;
            uint32_t : 14;
        } b;
    } MMCRIMR;
    union MMCTIMR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 14;
            uint32_t TGFSCM : 1;
            uint32_t TGFMSCM : 1;
            uint32_t : 5;
            uint32_t TGFM : 1;
            uint32_t : 10;
        } b;
    } MMCTIMR;
    uint32_t RESERVED3[14];
    uint32_t MMCTGFSCCR;
    uint32_t MMCTGFMSCCR;
    uint32_t RESERVED4[5];
    uint32_t MMCTGFCR;
    uint32_t RESERVED5[10];
    uint32_t MMCRFCECR;
    uint32_t MMCRFAECR;
    uint32_t RESERVED6[10];
    uint32_t MMCRGUFCR;
    uint32_t RESERVED7[334];
    union PTPTSCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TSE : 1;
            uint32_t TSFCU : 1;
            uint32_t TSSTI : 1;
            uint32_t TSSTU : 1;
            uint32_t TSITE : 1;
            uint32_t TSARU : 1;
            uint32_t : 10;
            uint32_t TSCNT : 2;
            uint32_t : 14;
        } b;
    } PTPTSCR;
    union PTPSSIR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t STSSI : 8;
            uint32_t : 24;
        } b;
    } PTPSSIR;
    uint32_t PTPTSHR;
    union PTPTSLR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t STSS : 31;
            uint32_t STPNS : 1;
        } b;
    } PTPTSLR;
    uint32_t PTPTSHUR;
    union PTPTSLUR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TSUSS : 31;
            uint32_t TSUPNS : 1;
        } b;
    } PTPTSLUR;
    uint32_t PTPTSAR;
    uint32_t PTPTTHR;
    uint32_t PTPTTLR;
    uint32_t RESERVED8;
    union PTPTSSR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 4;
            uint32_t TSSO : 1;
            uint32_t TSTTR : 1;
            uint32_t : 2;
            uint32_t TSSARFE : 1;
            uint32_t TSSSR : 1;
            uint32_t TSPTPPSV2E : 1;
            uint32_t TSSPTPOEFE : 1;
            uint32_t TSSIPV6FE : 1;
            uint32_t TSSIPV4FE : 1;
            uint32_t TSSEME : 1;
            uint32_t TSSMRME : 1;
            uint32_t : 16;
        } b;
    } PTPTSSR;
    uint32_t RESERVED9[565];
    union DMABMR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t SR : 1;
            uint32_t DA : 1;
            uint32_t DSL : 5;
            uint32_t EDE : 1;
            uint32_t PBL : 6;
            enum RTPR
            {
                RTPR_1_1 = 0x0,
                RTPR_2_1 = 0x1,
                RTPR_3_1 = 0x2,
                RTPR_4_1 = 0x3,
            } RTPR : 2;
            uint32_t FB : 1;
            uint32_t RDP : 6;
            uint32_t USP : 1;
            uint32_t FPM : 1;
            uint32_t AAB : 1;
            uint32_t : 6;
        } b;
    } DMABMR;
    uint32_t DMATPDR;
    uint32_t DMARPDR;
    uint32_t DMARDLAR;
    uint32_t DMATDLAR;
    union DMASR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TS : 1;
            uint32_t TPSS : 1;
            uint32_t TBUS : 1;
            uint32_t TJTS : 1;
            uint32_t ROS : 1;
            uint32_t TUS : 1;
            uint32_t RS : 1;
            uint32_t RBUS : 1;
            uint32_t RPSS : 1;
            uint32_t RWTS : 1;
            uint32_t ETS : 1;
            uint32_t : 2;
            uint32_t FBES : 1;
            uint32_t ERS : 1;
            uint32_t AIS : 1;
            uint32_t NIS : 1;
            enum RPS
            {
                RPS_Stopped = 0x0,
                RPS_Fetching = 0x1,
                RPS_Waiting = 0x3,
                RPS_Suspended = 0x4,
                RPS_Closing = 0x5,
                RPS_Queuing = 0x7,
            } RPS : 3;
            enum TPS
            {
                TPS_Stopped = 0x0,
                TPS_Fetching = 0x1,
                TPS_Waiting = 0x2,
                TPS_Reading = 0x3,
                TPS_Suspended = 0x6,
                TPS_Closing = 0x7,
            } TPS : 3;
            enum EBS
            {
                EBS_DataTransfTx = 0x1,
                EBS_ReadTransf = 0x2,
                EBS_DescAccess = 0x4,
            } EBS : 3;
            uint32_t : 1;
            uint32_t MMCS : 1;
            uint32_t PMTS : 1;
            uint32_t TSTS : 1;
            uint32_t : 2;
        } b;
    } DMASR;
    union DMAOMR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t : 1;
            uint32_t SR : 1;
            uint32_t OSF : 1;
            enum RTC
            {
                RTC_64Bytes = 0x0,
                RTC_32Bytes = 0x1,
                RTC_96Bytes = 0x2,
                RTC_128Bytes = 0x3,
            } RTC : 2;
            uint32_t : 1;
            uint32_t FUGF : 1;
            uint32_t FEF : 1;
            uint32_t : 5;
            uint32_t ST : 1;
            enum TTC
            {
                TTC_64Bytes = 0x0,
                TTC_128Bytes = 0x1,
                TTC_192Bytes = 0x2,
                TTC_256Bytes = 0x3,
                TTC_40Bytes = 0x4,
                TTC_32Bytes = 0x5,
                TTC_24Bytes = 0x6,
                TTC_16Bytes = 0x7,
            } TTC : 3;
            uint32_t : 3;
            uint32_t FTF : 1;
            uint32_t TSF : 1;
            uint32_t : 2;
            uint32_t DFRF : 1;
            uint32_t RSF : 1;
            uint32_t DTCEFD : 1;
            uint32_t : 5;
        } b;
    } DMAOMR;
    union DMAIER
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t TIE : 1;
            uint32_t TPSIE : 1;
            uint32_t TBUIE : 1;
            uint32_t TJTIE : 1;
            uint32_t ROIE : 1;
            uint32_t TUIE : 1;
            uint32_t RIE : 1;
            uint32_t RBUIE : 1;
            uint32_t RPSIE : 1;
            uint32_t RWTIE : 1;
            uint32_t ETIE : 1;
            uint32_t : 2;
            uint32_t FBEIE : 1;
            uint32_t ERIE : 1;
            uint32_t AISE : 1;
            uint32_t NISE : 1;
            uint32_t : 15;
        } b;
    } DMAIER;
    union DMAMFBOCR
    {
        uint32_t d32;
        struct __attribute__((packed))
        {
            uint32_t MFC : 16;
            uint32_t OMFC : 1;
            uint32_t MFA : 11;
            uint32_t OFOC : 1;
            uint32_t : 3;
        } b;
    } DMAMFBOCR;
    uint32_t DMARSWTR;
    uint32_t RESERVED10[8];
    uint32_t DMACHTDR;
    uint32_t DMACHRDR;
    uint32_t DMACHTBAR;
    uint32_t DMACHRBAR;
};

#define STM32_ETH_REGS (*((volatile STM32_ETH_REG_TYPE*)0x40028000))
