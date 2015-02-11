#include "global.h"
#include "../output.h"
#include "cpu/arm/cortexm/irq.h"
#include "soc/stm32/gpio.h"
#include "device/tm1829/stm32/tm1829_stm32.h"
#include "sys/util.h"


#define LED_PIN PIN_A4
#define LEDSTRIP_COUNT 2
#define LEDSTRIP_BASE { 0, 255 }
#define LEDSTRIP_LENGTH 170
#define LEDSTRIP_PIXELS_PER_IRQ 16
#define LEDSTRIP_TIME_SLOT 60
#define LEDSTRIP_TIME_HIGH 38
#define LEDSTRIP_TIME_LOW 14


static bool led_state = false;
static uint8_t led_data[LEDSTRIP_COUNT][LEDSTRIP_LENGTH][3];
static void* led_ptr[LEDSTRIP_COUNT];
static const uint16_t led_base[LEDSTRIP_COUNT] = LEDSTRIP_BASE;
static uint8_t dmabuf[24 * LEDSTRIP_PIXELS_PER_IRQ * LEDSTRIP_COUNT * 2];
static const TM1829::STM32Driver::Config ledConfig = { dmabuf, 0, 0, 1, LEDSTRIP_COUNT, LEDSTRIP_TIME_SLOT,
                                                       LEDSTRIP_TIME_LOW, LEDSTRIP_TIME_HIGH,
                                                       LEDSTRIP_PIXELS_PER_IRQ, 0, 4 };
static TM1829::STM32Driver ledStrip(&ledConfig);


void output_init()
{
    irq_set_priority(dma1_stream2_3_IRQn, 6);
    irq_enable(dma1_stream4_5_IRQn, true);
    for (int s = 0; s < LEDSTRIP_COUNT; s++)
    {
        uint8_t* ptr = *led_data[s];
        led_ptr[s] = (void**)ptr;
        for (int p = 0; p < LEDSTRIP_LENGTH; p++)
        {
            *ptr++ = 0xff;
            *ptr++ = 0xff;
            *ptr++ = 0xfe;
        }
    }
    ledStrip.sendFrame(led_ptr, LEDSTRIP_LENGTH);
}

void output_handle_dmx(uint8_t* data)
{
    while (ledStrip.isSending());
    for (int s = 0; s < LEDSTRIP_COUNT; s++)
    {
        memcpy(*led_data[s], data + led_base[s], MIN(sizeof(led_data[s]), (unsigned)(512 - led_base[s])));
        if (data[511] != 0xcc)
            for (int p = 0; p < LEDSTRIP_LENGTH; p++)
                if (led_data[s][p][0] == 0xff) led_data[s][p][0] = 0xfe;
        led_ptr[s] = (void**)*led_data[s];
    }
    ledStrip.sendFrame(led_ptr, LEDSTRIP_LENGTH);
    led_state ^= 1;
    GPIO::setLevel(LED_PIN, led_state);
}

extern "C" void dma1_stream4_5_irqhandler()
{
    ledStrip.handleIrq();
}
