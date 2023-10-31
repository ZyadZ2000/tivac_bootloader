#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define APP_START_ADDRESS ((uint32_t *)0x00002000U)

void BOOTLOADER_voidInit(void);
void BOOTLOADER_voidManager(void);
void BOOTLOADER_voidTask(void);

#endif
