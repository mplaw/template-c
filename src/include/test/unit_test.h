/// \file
/// A very simple unit test framework.
#pragma once

#include <stdlib.h>

#include "util/log.h"


/// @cond Doxygen_Suppress
#define STR_INNER(__EXPR__) #__EXPR__
//! @endcond


/// Convert \p \_\_EXPR\_\_ to a string literal.
#define STR(__EXPR__) STR_INNER(__EXPR__)


/// Exit if \p \_\_EXPR\_\_ evaluates to `false`.
#define CHECK(__EXPR__)                                                                            \
    do                                                                                             \
    {                                                                                              \
        if (!(__EXPR__))                                                                           \
        {                                                                                          \
            error("CHECK(%s) failed\n   in: %s\n   at: %s:%i\n", STR(__EXPR__), __func__,          \
                  __FILE__, __LINE__);                                                             \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)
