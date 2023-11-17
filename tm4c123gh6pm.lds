/******************************************************************************
 *
 * Modified Linker script for the Texas Instruments TM4C123GH6PM
 *
 * This is derived from revision 15071 of the TivaWare Library.
 *
 *****************************************************************************/

FLASH_START = 0x00000000;
FLASH_LENGTH = 0x00040000;

/* Limit the Bootloader to 8 KBs of Flash */
BOOTLOADER_START = FLASH_START;
BOOTLOADER_LENGTH = 0x00008000;

MEMORY
{
    FLASH (RX) : ORIGIN = BOOTLOADER_START, LENGTH = BOOTLOADER_LENGTH
    SRAM (WX)  : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

REGION_ALIAS("REGION_TEXT", FLASH);
REGION_ALIAS("REGION_BSS", SRAM);
REGION_ALIAS("REGION_DATA", SRAM);
REGION_ALIAS("REGION_STACK", SRAM);
REGION_ALIAS("REGION_HEAP", SRAM);
REGION_ALIAS("REGION_ARM_EXIDX", FLASH);
REGION_ALIAS("REGION_ARM_EXTAB", FLASH);

SECTIONS {

	/*
    .intvecs (_intvecs_base_address) : AT (_intvecs_base_address) {
        KEEP (*(.intvecs))
    } > REGION_TEXT
	*/

    /*
    PROVIDE (_vtable_base_address = 0x20000000);

    .vtable (_vtable_base_address) : AT (_vtable_base_address) {
        KEEP (*(.vtable))
    } > REGION_DATA
	*/

    .text : {
        __code_load__ = LOADADDR (.text);
        __code_start__ = .;
        CREATE_OBJECT_SYMBOLS
        KEEP (*(.intvecs))
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(0x4);
        KEEP (*(.ctors))
        . = ALIGN(0x4);
        KEEP (*(.dtors))
        . = ALIGN(0x4);
        __init_array_start = .;
        KEEP (*(.init_array*))
        __init_array_end = .;
        *(.init)
        *(.fini*)
        __code_end__ = .;
    } > SRAM AT> REGION_TEXT

    PROVIDE (_intvecs_base_address = __code_start__);
    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);
	/*
    .rodata : {
        *(.rodata)
        *(.rodata*)
    } > REGION_TEXT
	*/
    .data : ALIGN (4) {
        __data_load__ = LOADADDR (.data);
        __data_start__ = .;
        *(.data)
        *(.data*)
        . = ALIGN (4);
        __data_end__ = .;
    } > REGION_DATA AT> REGION_TEXT

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > REGION_ARM_EXIDX

    .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > REGION_ARM_EXTAB

    .bss : {
        __bss_start__ = .;
        *(.shbss)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end__ = .;
    } > REGION_BSS

    .heap : {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > REGION_HEAP


    .stack : ALIGN(0x8) {
        _stack = .;
        __stack = .;
        KEEP(*(.stack))
    } > REGION_STACK

   PROVIDE (end = .);
}
