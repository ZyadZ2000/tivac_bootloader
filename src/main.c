#include <stdbool.h>
#include <stdint.h>

#include "bootloader.h"

int main(void) {
  /* Initialize BootLoader */
  BOOTLOADER_voidInit();

  /* Manager: Jump to Application or stay in Boot Mode */
  BOOTLOADER_voidManager();

  /* BootLoader Task in case of Boot Mode */
  BOOTLOADER_voidTask();

  while (1) {
    /* Should never reach here */
  }

  return 0;
}
