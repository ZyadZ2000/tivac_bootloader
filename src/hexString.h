#ifndef CONVERTHEXSTRING_H
#define CONVERTHEXSTRING_H

#include <stdint.h>

#define HEX_STRING_CONVERT_SUCCESS 0
#define HEX_STRING_CONVERT_ERROR 1
#define HEX_STRING_LENGTH_INCORRECT 2

uint8_t HexString_u8_convert_to_int(const char *hex, uint8_t length,
                                    uint64_t *result);

#endif