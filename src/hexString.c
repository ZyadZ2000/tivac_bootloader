#include "hexString.h"
#include <assert.h>
#include <stddef.h>

uint8_t HexString_u8_convert_to_uint_bigEndian(const char *hex,
                                               const uint8_t length,
                                               uint8_t *result) {
  assert(hex != NULL && result != NULL);

  if (length > 16 || length <= 0) {
    return HEX_STRING_CONVERT_ERROR;
  }

  uint8_t iter = 0;
  *result = 0;

  if (hex[0] == '0' && hex[1] == 'x')
    hex = hex + 2;

  while (iter < length) {
    if (hex[iter] == '\0' || hex[iter] == '\n' || hex[iter] == '\r') {
      return HEX_STRING_LENGTH_INCORRECT;
    }

    for (uint8_t i = iter; i > 0; i--) {
      uint8_t offset = i / 2;
      uint8_t mod = i % 2;
      if (mod == 0) {
        *(result + offset) = (*(result + offset) & 0xF0) |
                             ((*(result + offset - 1) >> 4) & 0x0F);
      } else {
        *(result + offset) <<= 4;
      }
    }

    /* 0 - 9 */
    if (hex[iter] <= '9' && hex[iter] >= '0') {
      *result = (*result & 0xF0) | ((hex[iter] - 48) & 0x0F);

    }
    /* A - F */
    else if (hex[iter] <= 'F' && hex[iter] >= 'A') {
      *result = (*result & 0xF0) | ((hex[iter] - 55) & 0x0F);
    }
    /* a - f */
    else if (hex[iter] <= 'f' && hex[iter] >= 'a') {
      *result = (*result & 0xF0) | ((hex[iter] - 87) & 0x0F);
    } else {
      return HEX_STRING_CONVERT_ERROR;
    }

    iter++;
  }

  return HEX_STRING_CONVERT_SUCCESS;
}

uint64_t HexString_u64_convert_to_uint_littleEndian(
    const char *hex, const uint8_t length, uint8_t *const convert_state) {
  assert(hex != NULL && convert_state != NULL);

  if (length > 16 || length <= 0) {
    *convert_state = HEX_STRING_CONVERT_ERROR;
    return 0;
  }

  uint8_t iter = 0;
  uint64_t result = 0;
  uint8_t *bytePtr = (uint8_t *)&result;

  if (hex[0] == '0' && hex[1] == 'x')
    hex = hex + 2;

  while (iter < length) {
    if (hex[iter] == '\0' || hex[iter] == '\n' || hex[iter] == '\r') {
      *convert_state = HEX_STRING_LENGTH_INCORRECT;
      return 0;
    }

    if (iter % 2 != 0)
      *bytePtr = *bytePtr << 4;

    else if (iter != 0)
      bytePtr = bytePtr + 1;

    /* 0 - 9 */
    if (hex[iter] <= '9' && hex[iter] >= '0') {
      *bytePtr |= (hex[iter] - 48) & 0xF;
    }
    /* A - F */
    else if (hex[iter] <= 'F' && hex[iter] >= 'A') {
      *bytePtr |= (hex[iter] - 55) & 0xF;
    }
    /* a - f */
    else if (hex[iter] <= 'f' && hex[iter] >= 'a') {
      *bytePtr |= (hex[iter] - 87) & 0xF;
    } else {
      *convert_state = HEX_STRING_CONVERT_ERROR;
      return 0;
    }

    iter++;
  }

  *convert_state = HEX_STRING_CONVERT_SUCCESS;
  return result;
}
