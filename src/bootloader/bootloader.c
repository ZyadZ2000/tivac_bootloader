#include "bootloader.h"

#include <stdbool.h>
#include <stdint.h>

#include "eeprom.h"
#include "flash.h"
#include "gpio.h"
#include "hw_memmap.h"
#include "pin_map.h"
#include "rom.h"
#include "rom_map.h"
#include "srecord.h"
#include "sysctl.h"
#include "systick.h"
#include "tm4c123gh6pm.h"
#include "uart.h"

#define FLASH_END_ADDRESS 0x00040000U
#define APP_START_ADDRESS 0x00008000U

static void BootLoader_voidEraseFlash(void);
static void BootLoader_voidFlashRequest(void);
static void BootLoader_voidJumpToApp(void);
static void BootLoader_voidFlashImage(void);

uint8_t u8RxBuffer[255];

void BootLoader_voidInit(void)
{
    uint32_t u32EEPROMInit;

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    MAP_UARTDisable(UART0_BASE);

    MAP_UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);

    MAP_UARTConfigSetExpClk(
            UART0_BASE, MAP_SysCtlClockGet(), 9600,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

    MAP_UARTEnable(UART0_BASE);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

    while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0))
    {
    }

    u32EEPROMInit = EEPROMInit();

    if (u32EEPROMInit != EEPROM_INIT_OK)
    {
        while (1)
        {
        }
    }
}

void BootLoader_voidManager(void)
{
    /* Branching Checking */
    if (*((uint64_t*) APP_START_ADDRESS) != 0xFFFFFFFFFFFFFFFF)
    {
        /* Check if the shared byte in EEPROM is application mode */
        uint8_t sharedByte;
        EEPROMRead((uint32_t*) &sharedByte, 0x000, 4);
        if (sharedByte == 'A')
        {
            // /* Check If The Flashing Tool is Connected */
            // MAP_UARTCharPut(UART0_BASE, 'R');
            // /* Wait for the response within 1 second */
            // MAP_SysTickPeriodSet(16777216);
            // MAP_SysTickEnable();

            // /* Poll Until a UART Character is received or the SysTick finished
            //  * counting */
            // while ((UART0_FR_R & UART_FR_RXFE) && !(NVIC_ST_CTRL_R & 0x00010000))
            //   ;

            MAP_SysTickPeriodSet(160000);
            MAP_SysTickEnable();
            while ((UART0_FR_R & UART_FR_RXFE) && !(NVIC_ST_CTRL_R & 0x00010000))
                ;

            /* If there's no tool present jump into the application */
            if ((UART0_DR_R & 0xFF) != 'O')
            {
                BootLoader_voidJumpToApp();
            }
        }
    }
}

void BootLoader_voidTask(void)
{
    while (1)
    {
        uint8_t u8DataLen = MAP_UARTCharGet(UART0_BASE);
        uint16_t u16CalcChecksum = u8DataLen;

        for (uint8_t i = 0; i < u8DataLen; i++)
        {
            u8RxBuffer[i] = MAP_UARTCharGet(UART0_BASE);
            if (i < u8DataLen - 2)
                u16CalcChecksum += u8RxBuffer[i];
        }

        if (u8RxBuffer[u8DataLen - 1] != 0xEE)
        {
            /* Frame Error */
            MAP_UARTCharPut(UART0_BASE, 0xBB);
            MAP_UARTCharPut(UART0_BASE, 0xBB);
            MAP_UARTCharPut(UART0_BASE, 0xBB);
        }

        if ((~u16CalcChecksum & 0xFF) != u8RxBuffer[u8DataLen - 2])
        {
            /* Checksum Error */
            MAP_UARTCharPut(UART0_BASE, 0xBB);
            MAP_UARTCharPut(UART0_BASE, 0xAA);
            MAP_UARTCharPut(UART0_BASE, 0xAA);
        }

        switch (u8RxBuffer[0])
        {
        case 0x01:
        {
            BootLoader_voidEraseFlash();
            break;
        }
        case 0x02:
        {
            BootLoader_voidFlashRequest();
            break;
        }
        case 0x03:
        {
            BootLoader_voidJumpToApp();
            break;
        }
        default:
            /* Invalid Command */
            MAP_UARTCharPut(UART0_BASE, 0xBB);
            MAP_UARTCharPut(UART0_BASE, 0xCC);
            MAP_UARTCharPut(UART0_BASE, 0xCC);
        }
    }
}

static void BootLoader_voidEraseFlash(void)
{
    uint32_t u32FlashAddress = APP_START_ADDRESS;
    while (u32FlashAddress < FLASH_END_ADDRESS)
    {
        if (MAP_FlashErase(u32FlashAddress) != 0)
        {
            /* Erase Opeartion Failed */
            MAP_UARTCharPut(UART0_BASE, 0xBB);
            MAP_UARTCharPut(UART0_BASE, 0x01);
            MAP_UARTCharPut(UART0_BASE, 0x01);
            return;
        }

        u32FlashAddress += 1024;
    }

    /* Erase Opeartion Success */
    MAP_UARTCharPut(UART0_BASE, 0xBB);
    MAP_UARTCharPut(UART0_BASE, 0x01);
    MAP_UARTCharPut(UART0_BASE, 0x00);
}

static void BootLoader_voidFlashRequest(void)
{
    if (*((uint64_t*) APP_START_ADDRESS) != 0xFFFFFFFFFFFFFFFF)
    {
        /* Application is already flashed */
        MAP_UARTCharPut(UART0_BASE, 0xBB);
        MAP_UARTCharPut(UART0_BASE, 0x02);
        MAP_UARTCharPut(UART0_BASE, 0x01);
        return;
    }
    if (u8RxBuffer[1] == 0x00)
    {
        /* Aprove Request */
        MAP_UARTCharPut(UART0_BASE, 0xBB);
        MAP_UARTCharPut(UART0_BASE, 0x02);
        MAP_UARTCharPut(UART0_BASE, 0x00);
        BootLoader_voidFlashImage();
    }
}

static void BootLoader_voidFlashImage(void)
{
    uint32_t address = 0x00008000;
    while (1)
    {
        if (MAP_UARTCharGet(UART0_BASE) != 0xAA)
            return;

        for (uint8_t i = 0; i < 4; i++)
        {
            u8RxBuffer[i] = MAP_UARTCharGet(UART0_BASE);
        }

        if (MAP_FlashProgram((uint32_t*) u8RxBuffer, address, 4) != 0)
        {
            MAP_UARTCharPut(UART0_BASE, 0xBB);
            MAP_UARTCharPut(UART0_BASE, 0xEE);
            MAP_UARTCharPut(UART0_BASE, 0xEE);
            return;
        }
        address += 4;
        if (address == FLASH_END_ADDRESS)
        {
            MAP_UARTCharPut(UART0_BASE, 0xBB);
            MAP_UARTCharPut(UART0_BASE, 0xEE);
            MAP_UARTCharPut(UART0_BASE, 0xEE);
            return;
        }
        MAP_UARTCharPut(UART0_BASE, 0xBB);
        MAP_UARTCharPut(UART0_BASE, 0xAA);
        MAP_UARTCharPut(UART0_BASE, 0xAA);
    }
}

static void BootLoader_voidJumpToApp(void)
{
    if (*((uint64_t*) APP_START_ADDRESS) == 0xFFFFFFFFFFFFFFFF)
    {
        MAP_UARTCharPut(UART0_BASE, 0xBB);
        MAP_UARTCharPut(UART0_BASE, 0x03);
        MAP_UARTCharPut(UART0_BASE, 0x01);
        return;
    }
    MAP_IntMasterDisable();
    /* De-initialize Peripherals */
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_UART0);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_UART0);

    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOA);

    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EEPROM0);
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_EEPROM0);

    MAP_SysTickDisable();

    MAP_IntMasterEnable();
    /* Load Application Vector Table, Stack Pointer, and Jump */
    __asm(
            /* Update vector table offset to application vector table */
            "ldr     r0, = 0x00008000\n"
            "ldr     r1, = 0xe000ed08\n"
            "str     r0, [r1]\n"

            /* Update stack pointer from application vector table. First entry
             of vector table is SP */
            "ldr     r1, [r0]\n"
            "mov     sp, r1\n"

            /* Load application reset handler and jump to the user code */
            "ldr     r0, [r0, #4]\n"
            "bx      r0\n");
}

