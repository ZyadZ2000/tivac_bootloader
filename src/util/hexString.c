#include <assert.h>
#include <src/util/hexString.h>
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

uint8_t HexString_u8_convert_to_uint_littleEndian(const char *hex,
                                                  const uint8_t length,
                                                  uint8_t *result) {
  assert(hex != NULL && result != NULL);

  if (length > 16 || length <= 0) {
    return HEX_STRING_CONVERT_ERROR;
  }

  uint8_t iter = 0;

  if (hex[0] == '0' && hex[1] == 'x')
    hex = hex + 2;

  while (iter < length) {
    if (hex[iter] == '\0' || hex[iter] == '\n' || hex[iter] == '\r') {
      return HEX_STRING_LENGTH_INCORRECT;
    }

    if (iter % 2 != 0) {
      *result = *result << 4;
    } else if (iter != 0) {
      result = result + 1;
    }

    /* 0 - 9 */
    if (hex[iter] <= '9' && hex[iter] >= '0') {
      *result |= (hex[iter] - 48) & 0xF;
    }
    /* A - F */
    else if (hex[iter] <= 'F' && hex[iter] >= 'A') {
      *result |= (hex[iter] - 55) & 0xF;
    }
    /* a - f */
    else if (hex[iter] <= 'f' && hex[iter] >= 'a') {
      *result |= (hex[iter] - 87) & 0xF;
    } else {
      return HEX_STRING_CONVERT_ERROR;
    }

    iter++;
  }

  return HEX_STRING_CONVERT_SUCCESS;
}
