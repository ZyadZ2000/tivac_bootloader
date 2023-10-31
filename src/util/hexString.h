#ifndef CONVERTHEXSTRING_H
#define CONVERTHEXSTRING_H

#include <stdint.h>

#define HEX_STRING_CONVERT_SUCCESS 0
#define HEX_STRING_CONVERT_ERROR 1
#define HEX_STRING_LENGTH_INCORRECT 2

uint8_t HexString_u8ConvertToUintBigEndian(const char *hex,
                                           const uint8_t length,
                                           uint8_t *result);

uint8_t HexString_u8ConvertToUintLittleEndian(const char *hex,
                                              const uint8_t length,
                                              uint8_t *result);

#endif  // CONVERTHEXSTRING_H
