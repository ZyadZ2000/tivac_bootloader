#ifndef SRECORD_H
#define SRECORD_H

#include <stdint.h>

typedef struct {
  uint8_t type;
  uint8_t count;
  uint32_t address;
  uint32_t data[8]; // Max data length is 32 bytes
  uint8_t checksum;
} strctSRecord_t;

/* SRecord Types */
#define SRECORD_TYPE_S0 0
#define SRECORD_TYPE_S1 1
#define SRECORD_TYPE_S2 2
#define SRECORD_TYPE_S3 3
#define SRECORD_TYPE_S5 4
#define SRECORD_TYPE_S7 5
#define SRECORD_TYPE_S8 6
#define SRECORD_TYPE_S9 7

/* Data Field Types */
#define SRECORD_DATA_BIG_ENDIAN 0
#define SRECORD_DATA_LITTLE_ENDIAN 1

/* Parse States */
#define SRECORD_PARSE_SUCCESS 0
#define SRECORD_PARSE_ERROR 1

/* Function Declarations */
uint8_t SRecord_u8_parse(const char *charPtrRecord,
                         strctSRecord_t *const strctSRecord, uint8_t u8DataType);

#endif
