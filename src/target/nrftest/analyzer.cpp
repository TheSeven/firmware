#include "global.h"
#include "analyzer.h"
#include "sys/time.h"
#include "sys/util.h"
#include "soc/stm32/gpio.h"
#include "soc/stm32/spi.h"
#include "device/pcd8544/pcd8544.h"
#include "protocol.h"


PCD8544 lcd(&STM32::SPI::SPI1, PIN_F0, 4000000, PIN_F1, 2, 3, 68);
RadioPacket packet;
int startTime;
int nextSecond;
uint32_t lastReceived;
uint32_t lastSuccess;


void handleReset()
{
    memset(packet.byte, 0, sizeof(packet));
    startTime = read_usec_timer();
    nextSecond = TIMEOUT_SETUP(0);
    lastReceived = 0;
    lastSuccess = 0;
}


void resetAnalyzer()
{
    packet.f.reset = true;
}


void initAnalyzer(const char* title)
{
    lcd.print(0, 0, lcd.defaultFont, 0, title);
    lcd.print(1, 0, lcd.defaultFont, 0, "Initializing...");
    resetAnalyzer();
}


void displayStats()
{
    if (TIMEOUT_EXPIRED(nextSecond))
    {
        int delta = read_usec_timer() - startTime;
        int avReceived = packet.f.received / (delta / 1000000 + 1);
        int avSuccess = packet.f.success / (delta / 1000000 + 1);
        lcd.printf(1, 0, lcd.defaultFont, false, "OK: %6d/%6d ", packet.f.success, packet.f.sent);
        lcd.printf(2, 0, lcd.defaultFont, false, "RX: %6d/%6d ", packet.f.received, packet.f.sent + packet.f.retrans);
        lcd.printf(3, 0, lcd.defaultFont, false, "ACK: %6d/%6d ", packet.f.acked, packet.f.retrans);
        lcd.printf(4, 0, lcd.defaultFont, false, "RPS: %6d/%6d ", packet.f.received - lastReceived, avReceived);
        lcd.printf(5, 0, lcd.defaultFont, false, "SPS: %6d/%6d ", packet.f.success - lastSuccess, avSuccess);
        nextSecond = TIMEOUT_SETUP(1000000);
        lastReceived = packet.f.received;
        lastSuccess = packet.f.success;
    }
}
