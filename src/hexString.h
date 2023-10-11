#ifndef CONVERTHEXSTRING_H
#define CONVERTHEXSTRING_H

#include <stdint.h>

#define HEX_STRING_CONVERT_SUCCESS 0
#define HEX_STRING_CONVERT_ERROR 1
#define HEX_STRING_LENGTH_INCORRECT 2

uint8_t HexString_u8_convert_to_uint_bigEndian(const char *hex,
                                               const uint8_t length,
                                               uint8_t *result);

uint64_t HexString_u64_convert_to_uint_littleEndian(
    const char *hex, const uint8_t length, uint8_t *const convert_state);

#endif // CONVERTHEXSTRING_H
