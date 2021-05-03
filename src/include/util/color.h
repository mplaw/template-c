/// \file
/// Add ANSI color codes to terminal output.
#pragma once

#include <stdbool.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


/// A widely-supported subset of the possible terminal colour codes.
enum ANSI_color_codes
{
    WHITE = 0,
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    LIGHT_GRAY,
};


/// Initialise the `color_` family of functions.
///
/// By default color is enabled for terminals and disabled for any file for
/// which \rst :man7:`~isatty.3` \rst_end returns `false`.
///
/// The `COLOR` environment variable can be used to override this behaviour:
/// when set to `ON` it will force color to be always enabled, and similarly
/// when set to `OFF` color will always be disabled regardless of the type of
/// \p file.
void color_init();


/// Write \p color to \p file so that any subsequent writes are coloured.
bool color_set(FILE * file, enum ANSI_color_codes color);


#ifdef __cplusplus
} // extern "C"
#endif
