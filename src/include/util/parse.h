/// \file
/// Functions for parsing strings to other types.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/// Convert \p string to a boolean and store the result to \p result.
/// \return `true` if the conversion succeeds and `false` otherwise.
bool parse_bool(char const * string, bool * result);


/// Convert \p string to an enum and store the result to \p result.
/// \return `true` if the conversion succeeds and `false` otherwise.
bool parse_enum(char const * string, int * result, char const * enum_names[],
                size_t enum_names_size);


/// Convert \p string to a 32-bit unsigned integer and store the result to \p result.
/// \return `true` if the conversion succeeds and `false` otherwise.
bool parse_uint32_t(char const * string, uint32_t * result);


/// Convert \p string to a 32-bit signed integer and store the result to \p result.
/// \return `true` if the conversion succeeds and `false` otherwise.
bool parse_int32_t(char const * string, int32_t * result);


#ifdef __cplusplus
} // extern "C"
#endif
