#include "srecord.h"
#include "hexString.h"
#include <stddef.h>

#define SRECORD_CHECKSUM_VALID 0
#define SRECORD_CHECKSUM_INVALID 1

/* Inline Functions Declarations */
static inline uint8_t SRecord_parse_s0(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);
static inline uint8_t SRecord_parse_s1(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);
static inline uint8_t SRecord_parse_s2(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);
static inline uint8_t SRecord_parse_s3(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);
static inline uint8_t SRecord_parse_s5(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);
static inline uint8_t SRecord_parse_s7(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);
static inline uint8_t SRecord_parse_s8(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);
static inline uint8_t SRecord_parse_s9(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord);

/* Static Functions Declarations */
static uint8_t
SRecord_u8_validate_checksum(const strctSRecord_t *const strctSRecord);

uint8_t SRecord_u8_parse(const char *charPtrRecord,
                         strctSRecord_t *const strctSRecord) {
  if (charPtrRecord == NULL || strctSRecord == NULL)
    return SRECORD_PARSE_ERROR;

  if (charPtrRecord[0] != 'S')
    return SRECORD_PARSE_ERROR;

  uint8_t res = SRECORD_PARSE_ERROR;
  switch (charPtrRecord[1]) {
  case '0':
    res = SRecord_parse_s0(charPtrRecord, strctSRecord);
    break;
  case '1':
    res = SRecord_parse_s1(charPtrRecord, strctSRecord);
    break;
  case '2':
    res = SRecord_parse_s2(charPtrRecord, strctSRecord);
    break;
  case '3':
    res = SRecord_parse_s3(charPtrRecord, strctSRecord);
    break;
  case '5':
    res = SRecord_parse_s5(charPtrRecord, strctSRecord);
    break;
  case '7':
    res = SRecord_parse_s7(charPtrRecord, strctSRecord);
    break;
  case '8':
    res = SRecord_parse_s8(charPtrRecord, strctSRecord);
    break;
  case '9':
    res = SRecord_parse_s9(charPtrRecord, strctSRecord);
    break;
  default:
    return SRECORD_PARSE_ERROR;
  }

  return res;
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

static inline uint8_t SRecord_parse_s0(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S0;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);
  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->count <= 0x03)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 4, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->address != 0x0000)
    return SRECORD_PARSE_ERROR;

  uint8_t dataErr = HexString_u8_convert_to_int(
      charPtrRecord + 8, 2 * (strctSRecord->count - 3),
      (uint64_t *)&strctSRecord->data);

  if (dataErr == HEX_STRING_CONVERT_ERROR ||
      dataErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumErr =
      HexString_u8_convert_to_int(charPtrRecord + 2 * (strctSRecord->count - 1),
                                  2, (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static inline uint8_t SRecord_parse_s1(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S1;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);
  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->count <= 0x03)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 4, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t dataErr = HexString_u8_convert_to_int(
      charPtrRecord + 8, 2 * (strctSRecord->count - 3),
      (uint64_t *)&strctSRecord->data);

  if (dataErr == HEX_STRING_CONVERT_ERROR ||
      dataErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

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

static inline uint8_t SRecord_parse_s2(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S2;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);
  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->count <= 0x03)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 6, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t dataErr = HexString_u8_convert_to_int(
      charPtrRecord + 10, 2 * (strctSRecord->count - 3),
      (uint64_t *)&strctSRecord->data);

  if (dataErr == HEX_STRING_CONVERT_ERROR ||
      dataErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumErr =
      HexString_u8_convert_to_int(charPtrRecord + 2 * (strctSRecord->count - 1),
                                  2, (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static inline uint8_t SRecord_parse_s3(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S3;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);
  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->count <= 0x03)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 8, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t dataErr = HexString_u8_convert_to_int(
      charPtrRecord + 12, 2 * (strctSRecord->count - 3),
      (uint64_t *)&strctSRecord->data);

  if (dataErr == HEX_STRING_CONVERT_ERROR ||
      dataErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumErr =
      HexString_u8_convert_to_int(charPtrRecord + 2 * (strctSRecord->count - 1),
                                  2, (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static inline uint8_t SRecord_parse_s5(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S5;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);

  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT || strctSRecord->count != 0x03)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 4, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  strctSRecord->data = 0x00000000;

  uint8_t checksumErr = HexString_u8_convert_to_int(
      charPtrRecord + 8, 2, (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static inline uint8_t SRecord_parse_s7(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S7;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);

  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT || strctSRecord->count != 0x05)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 8, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  strctSRecord->data = 0x00000000;

  uint8_t checksumErr = HexString_u8_convert_to_int(
      charPtrRecord + 12, 2, (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static inline uint8_t SRecord_parse_s8(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S7;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);

  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT || strctSRecord->count != 0x04)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 6, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  strctSRecord->data = 0x00000000;

  uint8_t checksumErr = HexString_u8_convert_to_int(
      charPtrRecord + 10, 2, (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static inline uint8_t SRecord_parse_s9(const char *charPtrRecord,
                                       strctSRecord_t *const strctSRecord) {
  strctSRecord->type = SRECORD_TYPE_S9;
  uint8_t countErr = HexString_u8_convert_to_int(
      charPtrRecord + 2, 2, (uint64_t *)&strctSRecord->count);

  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT || strctSRecord->count != 0x03)
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_int(
      charPtrRecord + 4, 4, (uint64_t *)&strctSRecord->address);
  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  strctSRecord->data = 0x00000000;

  uint8_t checksumErr = HexString_u8_convert_to_int(
      charPtrRecord + 8, 2, (uint64_t *)&strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID)
    return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}
