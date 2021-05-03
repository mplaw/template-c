/// \file
/// Functions for accessing environment variables.
#pragma once

#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/// Return codes for the `env_<name>` family of functions.
enum env_rc
{
    /// All good in da hood.
    ENV_RC_OK,
    /// The environment variable does not exist.
    ENV_RC_NO_VARIABLE,
    /// The environment variable exists but has a value that could not be parsed.
    ENV_RC_INVALID_VALUE,
};


/// Return `true` if the environment variable \p name is defined.
bool env_has(char const * name);


/// Read an environment variable and convert its value to a boolean.
///
/// If \p name is not found then \p result will not be set and an error code
/// will be returned.
///
/// If \p name is found then \p result will be set to:
///
///   - `true` if the value is one of: `1`, `on`, `true`.
///   - `false` if the value is one of: `0`, `off`, `false`.
///
/// Otherwise an error code will be returned.
///
/// Note that the comparisons are **not** case sensitive.
///
/// \param name The name of the environment variable.
/// \param result The address of the memory to store the converted boolean value.
/// \return One of the following codes describing what the function did:
///     - ::ENV_RC_OK: The environment variable \p name was found, converted to
///       a boolean and written to address pointed to by \p result.
///     - ::ENV_RC_NO_VARIABLE: If the environment variable does not exist.
///     - ::ENV_RC_INVALID_VALUE: If the value could not be converted to a bool.
enum env_rc env_get_bool(char const * name, bool * result);


/// Read an environment variable and convert its value to an enum.
///
/// Example usage:
/// \rst_block
/// .. code-block:: c
///
///     #include "util/env.h"
///     enum log_level { INFO, WARNING, ERROR };
///     char const * log_level_names[] = { "INFO", "WARNING", "ERROR" };
///     enum log_level level;
///     switch(env_get_enum("LOG_LEVEL", &level, log_level_names, 3))
///     {
///         case ENV_RC_OK: return level;
///         case ENV_RC_NO_VARIABLE: return INFO; // default value
///         case ENV_RC_INVALID_VALUE: exit(EXIT_FAILURE);
///     }
/// \rst_end
/// \param name The name of the environment variable.
/// \param result The enum to write the result too.
/// \param enum_names List of names of the enum fields in value order. If the
///        enum is not contiguous \p enum_names can be padded with `NULL`s to
///        bridge the gap.
/// \param enum_names_size The number of items in \p enum_names.
/// \return One of the following codes describing what the function did:
///     - ::ENV_RC_OK: The environment variable \p name was found, converted to
///       an enum value and written to address pointed to by \p result.
///     - ::ENV_RC_NO_VARIABLE: If the environment variable does not exist.
///     - ::ENV_RC_INVALID_VALUE: If the value of the environment variable was
///       not found in \p enum_names.
enum env_rc env_get_enum(char const * name, int * result, char const * enum_names[],
                         size_t enum_names_size);


#ifdef __cplusplus
} // extern "C"
#endif
