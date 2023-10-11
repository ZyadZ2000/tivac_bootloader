#include <stdint.h>

int bootloader_main(void) {

  // Initialize the system clock

  // Initialize the peripherals / components

  // Do the branching logic

  // If jumping to application, disable interrupts, deinitialize peripherals,and
  // relocate the vector table.
  while (1) {
    // If you get here, you are in boot mode.
  }

  return 0;
}