#include "global.h"

#ifndef SRAM_BASE
#define SRAM_BASE 0x20000000
#endif

#ifndef CCMRAM_BASE
#define CCMRAM_BASE 0x10000000
#endif

#ifndef FLASH_BASE
#define FLASH_BASE 0x08000000
#endif

#ifdef CCMRAM_SIZE
#define BSS_REGION CCMRAM
#else
#define BSS_REGION SRAM
#endif

#ifdef RUNNING_FROM_RAM
#define CODE_REGION SRAM
#define STACK_REGION LOWRAM
#else
#define CODE_REGION FLASH
#define STACK_REGION BSS_REGION
#endif

#define DMABSS_REGION SRAM

#ifndef STACK_SIZE
#define STACK_SIZE 0x1000
#endif

MEMORY
{
    FLASH (rx) : ORIGIN = FLASH_BASE, LENGTH = FLASH_SIZE
#ifdef CCMRAM_SIZE
    CCMRAM (rw) : ORIGIN = CCMRAM_BASE, LENGTH = CCMRAM_SIZE
#endif
#ifdef RUNNING_FROM_RAM
    LOWRAM (rwx) : ORIGIN = SRAM_BASE, LENGTH = DFU_RAM_SIZE
    SRAM (rwx) : ORIGIN = SRAM_BASE + DFU_RAM_SIZE, LENGTH = SRAM_SIZE - DFU_RAM_SIZE
#else
    SRAM (rwx) : ORIGIN = SRAM_BASE, LENGTH = SRAM_SIZE
#endif
}

_stack_size = STACK_SIZE;

ENTRY(_irqvectors)

SECTIONS
{

    .text :
    {
        _text = .;
        KEEP(*(.vectors))
        *(.text)
        *(.text.*)
        . = ALIGN(1 << CACHEALIGN_BITS);
        _init_array = .;
        KEEP(*(.init_array))
        KEEP(*(.init_array.*))
        _init_array_end = .;
        . = ALIGN(1 << CACHEALIGN_BITS);
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(1 << CACHEALIGN_BITS);
        *(.dmarodata)
        *(.dmarodata.*)
        . = ALIGN(1 << CACHEALIGN_BITS);
        _text_end = .;
    } >CODE_REGION

    _data_src = _text_end;
    
    .stack (NOLOAD) :
    {
        _stack = .;
        . = . + _stack_size;
        . = ALIGN(1 << CACHEALIGN_BITS);
        _stack_end = .;
    } >STACK_REGION

    .data : AT ( _data_src )
    {
        _data = .;
        *(.data)
        *(.data.*)
        . = ALIGN(1 << CACHEALIGN_BITS);
        *(.dmadata)
        *(.dmadata.*)
        . = ALIGN(1 << CACHEALIGN_BITS);
        _data_end = .;
    } >SRAM

    .bss (NOLOAD) :
    {
        _bss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
#ifndef CCMRAM_SIZE
        *(.dmabss)
        *(.dmabss.*)
#endif
        . = ALIGN(1 << CACHEALIGN_BITS);
        _bss_end = .;
    } >BSS_REGION

#ifdef CCMRAM_SIZE
    .dmabss (NOLOAD) :
    {
        _dmabss = .;
        *(.dmabss)
        *(.dmabss.*)
        . = ALIGN(1 << CACHEALIGN_BITS);
        _dmabss_end = .;
    } >DMABSS_REGION
#endif

    DISCARD :
    {
        *(.note.*)
    }

    .stab            0 : { *(.stab) }
    .stabstr         0 : { *(.stabstr) }
    .stab.excl       0 : { *(.stab.excl) }
    .stab.exclstr    0 : { *(.stab.exclstr) }
    .stab.index      0 : { *(.stab.index) }
    .stab.indexstr   0 : { *(.stab.indexstr) }
    .comment         0 : { *(.comment) }
    .debug           0 : { *(.debug) }
    .line            0 : { *(.line) }
    .debug_srcinfo   0 : { *(.debug_srcinfo) }
    .debug_sfnames   0 : { *(.debug_sfnames) }
    .debug_aranges   0 : { *(.debug_aranges) }
    .debug_pubnames  0 : { *(.debug_pubnames) }
    .debug_info      0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev    0 : { *(.debug_abbrev) }
    .debug_line      0 : { *(.debug_line) }
    .debug_frame     0 : { *(.debug_frame) }
    .debug_str       0 : { *(.debug_str) }
    .debug_loc       0 : { *(.debug_loc) }
    .debug_macinfo   0 : { *(.debug_macinfo) }
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }

}
