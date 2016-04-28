#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/timer_regs.h"
#include "cpu/arm/cortexm/irq.h"
#include "device/ws2811/stm32/ws2811_stm32.h"
#include "interface/clockgate/clockgate.h"
#include "lib/fatfs/fatfs.h"
#include "common.h"
#include "usb.h"
#include "serial.h"


#define FLASH false
#define FRAMETIME 20000
#define LEDSTRIP_LENGTH 300
#define LEDSTRIP_PIXELS_PER_IRQ 4


static union
{
    uint8_t pixelRow[LEDSTRIP_LENGTH][3];
    struct __attribute__((packed,aligned(4))) BMPHeader
    {
        uint16_t magic;
        uint16_t sizeLow;
        uint16_t sizeHigh;
        uint16_t : 16;
        uint16_t : 16;
        uint16_t dataOffsetLow;
        uint16_t dataOffsetHigh;
        uint16_t : 16;
        uint16_t : 16;
        uint16_t widthLow;
        uint16_t widthHigh;
        uint16_t heightLow;
        uint16_t heightHigh;
        uint16_t colorPlanes;
        uint16_t bitsPerPixel;
        uint16_t compMethodLow;
        uint16_t compMethodHigh;
        uint16_t dataSizeLow;
        uint16_t dataSizeHigh;
    } bmpHeader;
} buffer;
static uint8_t dmabuf[24 * LEDSTRIP_PIXELS_PER_IRQ * 2];
static void* outputPtr;
static bool restart_main_loop;

static const WS2811::STM32Driver::Config ledConfig = { dmabuf, 1, 0, 3, 1, 60, 12, 29, LEDSTRIP_PIXELS_PER_IRQ, 0, 1 };
static WS2811::STM32Driver ledStrip(&ledConfig);


static bool handle_external_access()
{
    if (usb_poll() || serial_poll()) restart_main_loop = true;
    return restart_main_loop;
}

static void pushPixels(int length)
{
    outputPtr = (void*)*buffer.pixelRow;
    ledStrip.sendFrame(&outputPtr, length);
}

static void setPixels(uint32_t pattern, uint32_t color)
{
    uint8_t* ptr = *buffer.pixelRow;
    for (int i = 0; i < 32; i++)
    {
        if (pattern & 1)
        {
            *ptr++ = color >> 8;
            *ptr++ = color >> 16;
            *ptr++ = color;
        }
        else
        {
            *ptr++ = 0;
            *ptr++ = 0;
            *ptr++ = 0;
        }
        pattern >>= 1;
    }
}

static void delay(int time)
{
    if (restart_main_loop) return;
    int timeout = TIMEOUT_SETUP(time);
    while (!TIMEOUT_EXPIRED(timeout) && !handle_external_access());
}

static void flash(uint32_t pattern, uint32_t color, uint32_t onTime, uint32_t offTime, uint32_t count)
{
    while (count--)
    {
        setPixels(pattern, color);
        pushPixels(LEDSTRIP_LENGTH);
        while (ledStrip.isSending());
        delay(onTime);
        setPixels(0x00, 0);
        pushPixels(LEDSTRIP_LENGTH);
        while (ledStrip.isSending());
        delay(offTime);
    }
}

static void display(const char* text)
{
    lcd.clear();
    lcd.print(0, 0, lcd.defaultFont, 0, text);
}

int main()
{
    irq_enable(dma1_stream2_3_dma2_stream1_2_IRQn, true);

    clockgate_enable(STM32_TIM16_CLOCKGATE, true);
    union STM32_TIM_REG_TYPE::CCMR1 CCMR1 = { 0 };
    CCMR1.oc.OC1PE = true;
    CCMR1.oc.OC1M = 6;
    STM32_TIM16_REGS.CCMR1.d32 = CCMR1.d32;
    union STM32_TIM_REG_TYPE::CCER CCER = { 0 };
    CCER.b.CC1E = true;
    CCER.b.CC1P = true;
    STM32_TIM16_REGS.CCER.d32 = CCER.d32;
    union STM32_TIM_REG_TYPE::BDTR BDTR = { 0 };
    BDTR.b.MOE = true;
    STM32_TIM16_REGS.BDTR.d32 = BDTR.d32;
    union STM32_TIM_REG_TYPE::CR2 CR2 = { 0 };
    STM32_TIM16_REGS.CR2.d32 = CR2.d32;
    STM32_TIM16_REGS.PSC = 0;
    STM32_TIM16_REGS.ARR = 47999;
    union STM32_TIM_REG_TYPE::EGR EGR = { 0 };
    STM32_TIM16_REGS.CCR1 = 10000;
    EGR.b.UG = true;
    STM32_TIM16_REGS.EGR.d32 = EGR.d32;
    union STM32_TIM_REG_TYPE::CR1 CR1 = { 0 };
    CR1.b.CEN = true;
    STM32_TIM16_REGS.CR1.d32 = CR1.d32;

    serial_init();
    usb_init();

    while (true)
    {
        restart_main_loop = false;
        handle_external_access();

        memset(&buffer, 0, sizeof(buffer));
        udelay(1000000);
        display("Unounting FS");
        FRESULT result;
        fs.~FileSystem();
        display("Mounting FS");
        new(&fs) FatFs::FileSystem(&sd, 0, "0:", &result);
        if (result != FR_OK) continue;
        display("Opening file");
        FatFs::FileMode mode = { 0 };
        mode.read = true;
        FatFs::File file("/sequence.bmp", mode, &result);
        if (result != FR_OK) continue;
        display("Reading data");
        uint32_t read;
        if (file.read(&buffer.bmpHeader, sizeof(buffer.bmpHeader), &read) != FR_OK) continue;
        display("Check read size");
        if (read != sizeof(buffer.bmpHeader)) continue;
        display("Check header");
        if (buffer.bmpHeader.magic != 0x4d42) continue;
        display("Check planes");
        if (buffer.bmpHeader.colorPlanes != 1) continue;
        display("Check bpp");
        if (buffer.bmpHeader.bitsPerPixel != 24) continue;
        display("Check compression");
        if (((buffer.bmpHeader.compMethodHigh << 16) | buffer.bmpHeader.compMethodLow) != 0) continue;
        uint32_t fileSize = (buffer.bmpHeader.sizeHigh << 16) | buffer.bmpHeader.sizeLow;
        uint32_t dataOffset = (buffer.bmpHeader.dataOffsetHigh << 16) | buffer.bmpHeader.dataOffsetLow;
        uint32_t width = (buffer.bmpHeader.widthHigh << 16) | buffer.bmpHeader.widthLow;
        uint32_t height = (buffer.bmpHeader.heightHigh << 16) | buffer.bmpHeader.heightLow;
        uint32_t dataSize = (buffer.bmpHeader.dataSizeHigh << 16) | buffer.bmpHeader.dataSizeLow;
        uint32_t stride = ((width * 3 + 3) >> 2) << 2;
        uint32_t pixels = MIN(LEDSTRIP_LENGTH, width);
        display("Check data size");
        if (dataSize < height * stride) continue;
        display("Check file size");
        if (fileSize < dataOffset + dataSize) continue;

        display("Countdown");
        flash(0x000003c0, 0x101010, 0, 900000, 1);
        flash(0x00000038, 0x101010, 0, 900000, 1);
        flash(0x00000006, 0x101010, 0, 900000, 1);
        flash(0x00000001, 0x101010, 0, 900000, 1);
        if (restart_main_loop) continue;

        display("Playing");
        int nextFrame = TIMEOUT_SETUP(0);
        for (uint32_t row = 0; row < height; row++)
        {
            file.lseek(dataOffset + row * stride);
            while (ledStrip.isSending());

            if (FLASH)
            {
                memset(&buffer, 0, sizeof(buffer));
                pushPixels(pixels);
                while (ledStrip.isSending());
            }

            if (handle_external_access()) break;
            if (file.read(*buffer.pixelRow, 3 * pixels, &read) != FR_OK) break;
            if (read != 3 * pixels) break;
            for (uint32_t i = 0; i < pixels; i++)
            {
                uint8_t tmp = buffer.pixelRow[i][0];
                buffer.pixelRow[i][0] = buffer.pixelRow[i][1];
                buffer.pixelRow[i][1] = buffer.pixelRow[i][2];
                buffer.pixelRow[i][2] = tmp;
            }

            if (handle_external_access()) break;
            while (!TIMEOUT_EXPIRED(nextFrame));
            nextFrame = TIMEOUT_SETUP(FRAMETIME);
            pushPixels(pixels);
        }

        while (ledStrip.isSending());
        memset(&buffer, 0, sizeof(buffer));
        if (!FLASH) while (!TIMEOUT_EXPIRED(nextFrame));
        pushPixels(LEDSTRIP_LENGTH);
    }
}

extern "C" void dma1_stream2_3_dma2_stream1_2_irqhandler()
{
    ledStrip.handleIrq();
}
