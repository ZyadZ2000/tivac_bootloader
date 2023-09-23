#include "packet.h"

void packet_init(void) {}

packet_state_t read_and_check_packet(int8_t packet) {
  static int8_t currState = 0;
  switch (currState) {}
}