#include "srecord.h"
#include "hexString.h"
#include <stddef.h>

#define SRECORD_CHECKSUM_VALID 0
#define SRECORD_CHECKSUM_INVALID 1

/* Array holding S-Record Type specific properties. */
const uint8_t min_count[] = {4, 4, 5, 6, 3, 5, 4, 3};
const uint8_t max_count[] = {7, 7, 8, 9, 3, 5, 4, 3};
const uint8_t address_length[] = {4, 4, 6, 8, 4, 8, 6, 4};

/* Static Functions Declarations */
static uint8_t
SRecord_u8_validate_checksum(const strctSRecord_t *const strctSRecord);

uint8_t SRecord_u8_parse(const char *charPtrRecord,
                         strctSRecord_t *const strctSRecord) {
  if (charPtrRecord == NULL || strctSRecord == NULL)
    return SRECORD_PARSE_ERROR;

  if (charPtrRecord[0] != 'S')
    return SRECORD_PARSE_ERROR;

  switch (charPtrRecord[1]) {
  case '0':
    strctSRecord->type = SRECORD_TYPE_S0;
    break;
  case '1':
    strctSRecord->type = SRECORD_TYPE_S1;
    break;
  case '2':
    strctSRecord->type = SRECORD_TYPE_S2;
    break;
  case '3':
    strctSRecord->type = SRECORD_TYPE_S3;
    break;
  case '5':
    strctSRecord->type = SRECORD_TYPE_S5;
    break;
  case '7':
    strctSRecord->type = SRECORD_TYPE_S7;
    break;
  case '8':
    strctSRecord->type = SRECORD_TYPE_S8;
    break;
  case '9':
    strctSRecord->type = SRECORD_TYPE_S9;
    break;
  default:
    return SRECORD_PARSE_ERROR;
    break;
  }

  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);
  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->count < min_count[strctSRecord->type] ||
      strctSRecord->count > max_count[strctSRecord->type])
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, address_length[strctSRecord->type],
      (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->type != SRECORD_TYPE_S5 &&
      strctSRecord->type != SRECORD_TYPE_S7 &&
      strctSRecord->type != SRECORD_TYPE_S8 &&
      strctSRecord->type != SRECORD_TYPE_S9) {
    uint8_t dataErr = HexString_u8_convert_to_int(
        charPtrRecord + 4 + address_length[strctSRecord->type],
        2 * (strctSRecord->count - 3), (uint64_t *)&strctSRecord->data);

    if (dataErr == HEX_STRING_CONVERT_ERROR ||
        dataErr == HEX_STRING_LENGTH_INCORRECT)
      return SRECORD_PARSE_ERROR;
  } else {
    strctSRecord->data = 0x00000000;
  }

  uint8_t checksumErr = HexString_u8_convert_to_int(
      charPtrRecord + 4 + 2 * (strctSRecord->count - 1), 2,
      (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static uint8_t
SRecord_u8_validate_checksum(const strctSRecord_t *const strctSRecord) {
  if (strctSRecord == NULL)
    return SRECORD_CHECKSUM_INVALID;
  uint16_t checksum = 0;
  checksum += strctSRecord->count;
  int i;
  for (i = 0; i < 4; i++) {
    checksum += (strctSRecord->address >> (8 * i)) & 0xFF;
    checksum += (strctSRecord->data >> (8 * i)) & 0xFF;
  }
  checksum = ~checksum;
  if ((checksum & 0xFF) != strctSRecord->checksum)
    return SRECORD_CHECKSUM_INVALID;

  return SRECORD_CHECKSUM_VALID;
}