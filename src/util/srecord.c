#include <src/util/hexString.h>
#include <src/util/srecord.h>
#include <stddef.h>
#include <stdio.h>


#define SRECORD_CHECKSUM_VALID 0
#define SRECORD_CHECKSUM_INVALID 1

/* Array holding S-Record Type specific properties. */
const uint8_t min_count[] = {4, 4, 5, 6, 3, 5, 4, 3};
const uint8_t max_count[] = {35, 35, 36, 37, 3, 5, 4, 3};
const uint8_t address_length[] = {4, 4, 6, 8, 4, 8, 6, 4};

/* Static Functions Declarations */
static uint8_t SRecord_u8_validate_checksum(
    const strctSRecord_t* const strctSRecord);

uint8_t SRecord_u8_parse(const char* charPtrRecord,
                         strctSRecord_t* const strctSRecord,
                         const uint8_t u8DataType) {
  if (charPtrRecord == NULL || strctSRecord == NULL) return SRECORD_PARSE_ERROR;

  if (charPtrRecord[0] != 'S') return SRECORD_PARSE_ERROR;

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

  uint8_t countErr = HexString_u8_convert_to_uint_bigEndian(
      charPtrRecord + 2, 2, &strctSRecord->count);

  if (countErr == HEX_STRING_CONVERT_ERROR ||
      countErr == HEX_STRING_LENGTH_INCORRECT) {
    return SRECORD_PARSE_ERROR;
  }

  if (strctSRecord->count < min_count[strctSRecord->type] ||
      strctSRecord->count > max_count[strctSRecord->type])
    return SRECORD_PARSE_ERROR;

  uint8_t addErr = HexString_u8_convert_to_uint_bigEndian(
      charPtrRecord + 4, address_length[strctSRecord->type],
      (uint8_t*)&strctSRecord->address);

  if (addErr == HEX_STRING_CONVERT_ERROR ||
      addErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  if (strctSRecord->type != SRECORD_TYPE_S5 &&
      strctSRecord->type != SRECORD_TYPE_S7 &&
      strctSRecord->type != SRECORD_TYPE_S8 &&
      strctSRecord->type != SRECORD_TYPE_S9) {
    int8_t dataCount =
        strctSRecord->count - (address_length[strctSRecord->type] / 2) - 1;

    uint8_t dataErr = 0;
    uint8_t i = 0;
    do {
      uint8_t charCount = 0;

      if (dataCount < 4) {
        charCount = dataCount * 2;
      } else {
        charCount = 8;
      }

      if (u8DataType == SRECORD_DATA_BIG_ENDIAN) {
        dataErr = HexString_u8_convert_to_uint_bigEndian(
            charPtrRecord + 4 + address_length[strctSRecord->type] + (8 * i),
            charCount, (uint8_t*)&strctSRecord->data[i]);

      } else if (u8DataType == SRECORD_DATA_LITTLE_ENDIAN) {
        dataErr = HexString_u8_convert_to_uint_littleEndian(
            charPtrRecord + 4 + address_length[strctSRecord->type] + (8 * i),
            charCount, (uint8_t*)&strctSRecord->data[i]);

      } else {
        return SRECORD_PARSE_ERROR;
      }

      if (dataErr == HEX_STRING_CONVERT_ERROR ||
          dataErr == HEX_STRING_LENGTH_INCORRECT)
        return SRECORD_PARSE_ERROR;

      i++;
      dataCount = dataCount - 4;
    } while (dataCount > 0);
  }

  // } else {
  //   for (uint8_t i = 0; i < 8; i++) {
  //     strctSRecord->data[i] = 0x00000000;
  //   }
  // }

  uint8_t checksumErr = HexString_u8_convert_to_uint_bigEndian(
      charPtrRecord + 4 + 2 * (strctSRecord->count - 1), 2,
      &strctSRecord->checksum);

  if (checksumErr == HEX_STRING_CONVERT_ERROR ||
      checksumErr == HEX_STRING_LENGTH_INCORRECT)
    return SRECORD_PARSE_ERROR;

  uint8_t checksumValid = SRecord_u8_validate_checksum(strctSRecord);

  if (checksumValid == SRECORD_CHECKSUM_INVALID) return SRECORD_PARSE_ERROR;

  return SRECORD_PARSE_SUCCESS;
}

static uint8_t SRecord_u8_validate_checksum(
    const strctSRecord_t* const strctSRecord) {
  if (strctSRecord == NULL) return SRECORD_CHECKSUM_INVALID;
  uint16_t checksum = 0;
  checksum += strctSRecord->count;
  int i, j;
  for (i = 0; i < 4; i++) {
    checksum += (strctSRecord->address >> (8 * i)) & 0xFF;
  }

  if (strctSRecord->type != SRECORD_TYPE_S5 &&
      strctSRecord->type != SRECORD_TYPE_S7 &&
      strctSRecord->type != SRECORD_TYPE_S8 &&
      strctSRecord->type != SRECORD_TYPE_S9) {
    uint8_t dataCount =
        strctSRecord->count - (address_length[strctSRecord->type] / 2) - 1;

    for (i = 0; i < dataCount; i += 4) {
      for (j = 0; j < 4; j++) {
        checksum += (strctSRecord->data[i / 4] >> (8 * j)) & 0xFF;
      }
    }
  }
  checksum = ~checksum;

  if ((checksum & 0xFF) != strctSRecord->checksum)
    return SRECORD_CHECKSUM_INVALID;

  return SRECORD_CHECKSUM_VALID;
}
