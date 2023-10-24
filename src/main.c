#include <stdbool.h>
#include <stdint.h>

#include "packet.h"
#include "sysctl.h"


int main(void) {
  /* Initialize and Config UART0 (communication with the serial flashing tool)
   */
  SysCtlClockGet();
  /* Branching logic */

  /* If jumping to application, disable interrupts, deinitialize
   peripherals,and relocate the vector table. */
  while (1) {
    /* Boot Mode*/
    /* Receive and Respond to Packets */
  }

  return 0;
}
