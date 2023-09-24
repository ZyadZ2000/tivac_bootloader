#include "hexString.h"
#include <assert.h>
#include <stddef.h>

uint8_t HexString_u8_convert_to_int(const char *hex, const uint8_t length,
                                    uint64_t *const result) {
  if (length > 16 || length <= 0 || hex == NULL || result == NULL)
    return HEX_STRING_CONVERT_ERROR;

  uint8_t iter = 0;
  *result = 0; // Initialize result to 0

  if (hex[0] == '0' && hex[1] == 'x')
    hex = hex + 2;

  while (iter < length) {
    if (hex[iter] == '\0')
      return HEX_STRING_LENGTH_INCORRECT;

    *result = *result << 4;
    /* 0 - 9 */
    if (hex[iter] <= '9' && hex[iter] >= '0')
      *result |= (hex[iter] - 48) & 0xFF;

    /* A - F */
    else if (hex[iter] <= 'F' && hex[iter] >= 'A')
      *result |= (hex[iter] - 55) & 0xFF;

    /* a - f */
    else if (hex[iter] <= 'f' && hex[iter] >= 'a')
      *result |= (hex[iter] - 87) & 0xFF;

    else
      return HEX_STRING_CONVERT_ERROR;

    iter++;
  }

  return HEX_STRING_CONVERT_SUCCESS;
}
