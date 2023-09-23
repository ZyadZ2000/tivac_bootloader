#ifndef SRECORD_H
#define SRECORD_H

#include <stdint.h>

typedef struct {
  uint8_t type;
  uint8_t count;
  uint32_t address;
  uint32_t data;
  uint8_t checksum;
} strctSRecord_t;

/* SRecord Types */
#define SRECORD_TYPE_S0 0
#define SRECORD_TYPE_S1 1
#define SRECORD_TYPE_S2 2
#define SRECORD_TYPE_S3 3
#define SRECORD_TYPE_S5 5
#define SRECORD_TYPE_S7 7
#define SRECORD_TYPE_S8 8
#define SRECORD_TYPE_S9 9

/* Parse States */
#define SRECORD_PARSE_SUCCESS 0
#define SRECORD_PARSE_ERROR 1

/* Function Declarations */
uint8_t SRecord_u8_parse(const char *charPtrRecord,
                         strctSRecord_t *const strctSRecord);

#endif