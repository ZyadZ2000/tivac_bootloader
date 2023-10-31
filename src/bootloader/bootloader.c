#include "bootloader.h"

#include <stdbool.h>
#include <stdint.h>

#include "eeprom.h"
#include "flash.h"
#include "gpio.h"
#include "hw_memmap.h"
#include "packet.h"
#include "pin_map.h"
#include "rom.h"
#include "rom_map.h"
#include "sysctl.h"
#include "systick.h"
#include "tm4c123gh6pm.h"
#include "uart.h"

uint8_t gSystickFlag = 0;
int8_t gUartByte = 0;

void SysTick_IRQ_Handler(void) { gSystickFlag = 1; }

void UART0_IRQ_Handler(void) { gUartByte = UART0_DR_R & 0x0FF; }

void BOOTLOADER_voidInit(void) {
  uint32_t u32EEPROMInit;

  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {
  }

  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {
  }

  MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
  MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
  MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  MAP_UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);

  MAP_UARTDisable(UART0_BASE);

  MAP_UARTConfigSetExpClk(
      UART0_BASE, MAP_SysCtlClockGet(), 9600,
      UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

  MAP_UARTEnable(UART0_BASE);

  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

  while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0)) {
  }

  u32EEPROMInit = EEPROMInit();

  if (u32EEPROMInit != EEPROM_INIT_OK) {
    while (1) {
    }
  }
}

void BOOTLOADER_voidManager(void) {
  /* Branching Checking */
  if (*((uint64_t *)APP_START_ADDRESS) != 0xFFFFFFFFFFFFFFFF) {
    /* Check if the shared byte in EEPROM is application mode */
    uint8_t sharedByte;
    EEPROMRead((uint32_t *)&sharedByte, 0x000, 1);
    if (sharedByte == 'A') {
      /* Check If The Flashing Tool is Connected */
      MAP_IntMasterEnable();
      MAP_IntEnable(INT_UART0);
      MAP_UARTCharPut(UART0_BASE, 'R');
      MAP_UARTIntEnable(UART0_BASE, UART_INT_RX);
      /* Wait for the response within 1 second */
      MAP_SysTickPeriodSet(16777216);
      MAP_SysTickIntEnable();
      MAP_SysTickEnable();
      while (gUartByte != 'O' && gSystickFlag == 0)
        ;

      /* If there's no tool present jump into the application */
      if (gUartByte != 'O') {
        /* De-initialize Peripherals */
        MAP_IntDisable(INT_UART0); /* Disable UART0 interrupt from NVIC */
        MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_UART0);
        MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_UART0);

        MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOA);
        MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOA);

        MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EEPROM0);
        MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_EEPROM0);

        MAP_SysTickIntDisable();
        MAP_SysTickDisable();
        /* Load Application Vector Table, Stack Pointer, and Jump */
        __asm(
            /* Update vector table offset to application vector table */
            "ldr     r0, = 0x00002000\n"
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
    }
  }
}

void BOOTLOADER_voidTask(void) {
  while (1) {
  }
}
