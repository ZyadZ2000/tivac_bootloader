#include <stdbool.h>
#include <stdint.h>

#include "bootloader.h"

int main(void) {
  /* Initialize BootLoader */
  BootLoader_voidInit();

  /* Manager: Jump to Application or stay in Boot Mode */
  BootLoader_voidManager();

  /* BootLoader Task in case of Boot Mode */
  BootLoader_voidTask();

  while (1) {
    /* Should never reach here */
  }

  return 0;
}
