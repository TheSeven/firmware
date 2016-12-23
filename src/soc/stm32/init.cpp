#include "global.h"
#include "soc/stm32/syscfg.h"
#include "cpu/arm/cortexm/init.h"
#include "cpu/arm/cortexm/irq.h"
#include "cpu/arm/cortexm/time.h"
#include "cpu/arm/cortexm/cmsis.h"
#include "sys/init.h"
#include "sys/util.h"
#include "sys/time.h"

#if defined(SOC_STM32F0)
#include "soc/stm32/gpio.h"
#include "soc/stm32/f0/rcc.h"
#elif defined(SOC_STM32F1)
#include "soc/stm32/f1/gpio.h"
#include "soc/stm32/f1/rcc.h"
#elif defined(SOC_STM32F2) || defined(SOC_STM32F4)
#include "soc/stm32/gpio.h"
#include "soc/stm32/f2/rcc.h"
#endif

using namespace STM32;


// These aren't variables, but actually just symbols. Declare them as bytes to make pointer arithmetic work.
extern "C" uint8_t _text, _text_end, _data, _data_end, _data_src, _bss, _bss_end, _dmabss, _dmabss_end;


void __attribute__((noreturn)) init_stage2();

// This is the reset vector entry point
void __attribute__((weak,noreturn)) init()
{
#ifndef CPU_ARM_CORTEX_M0
    // Set up IRQ vector table location
    SCB->VTOR = (uint32_t)_irqvectors;
#endif

    // Initialize system clocking
    RCC::init();
    
#ifndef SOC_STM32F1
    // Initialize memory mapping
    SYSCFG::init();
#endif
    
#ifdef MOVE_CODE_INTO_PLACE
    // Copy .text and .data sections to their destination addresses
    void* origin, tmp;
    asm("get_origin: ADR %0, get_origin; LDR %1, =_text - get_origin; ADD %0, %0, %1" : "=l"(origin), "=l"(tmp));
    memmove(&_text, origin, &_data_end - &_text);
#endif

    // Jump to stage 2 at the correct address
    __attribute__((noreturn)) void (*stage2)() = (TYPE_NORETURN(*)())(((uint32_t)init_stage2) | 1);
    stage2();
}

void __attribute__((noreturn)) init_stage2()
{
    // Initialize R/W data segment from flash if required
#ifdef RUNNING_FROM_FLASH
    memcpy(&_data, &_data_src, &_data_end - &_data);
#endif

    // Clear BSS section
    memset(&_bss, 0, &_bss_end - &_bss);
    
#ifdef CCMRAM_SIZE
    // Clear DMABSS section
    memset(&_dmabss, 0, &_dmabss_end - &_dmabss);
#endif

    // Disable stack frame alignment
    SCB->CCR = 0;

#ifdef SOC_STM32F4
    // Enable FPU access for everyone
    SCB->CPACR |= (3 << 20) | (3 << 22);
#endif

    // Initialize I/O pins
#ifdef SOC_STM32F1
    STM32::GPIO_F1::init();
#else
    STM32::GPIO::init();
#endif

    // Disable all IRQ sources
    for (uint32_t i = 0; i < ARRAYLEN(NVIC->ICER); i++) NVIC->ICER[i] = 0xffffffff;

    // Enable IRQs
    leave_critical_section();

    // Initialize the system tick IRQ and microsecond timer
    time_init();

    // Initialize runtime system and run main application program
    sys_init();
    
    // If main ever returns, power down the CPU
    powerdown();
}

