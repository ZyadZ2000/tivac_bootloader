#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

typedef unsigned char packet_state_t;

#define PACKET_STATE_OK 0
#define PACKET_STATE_ERROR 1

packet_state_t read_and_check_packet(int8_t packet);

#endif