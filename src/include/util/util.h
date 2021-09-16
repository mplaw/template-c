/// \file
#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/// Mark \p variable as unused to stop compiler warnings.
#define UNUSED(variable) (void)(variable)


/// Return the size of \p array (do not pass this a pointer!).
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))


/// Tell the compiler \p condition is likely to be true.
#define LIKELY(condition) __builtin_expect((condition), 1)


/// Tell the compiler \p condition is unlikely to be true.
#define UNLIKELY(condition) __builtin_expect((condition), 0)


/// @cond Doxygen_Suppress
#define STR_INNER(expression) #expression
//! @endcond


/// Convert \p expression to a string literal.
#define STR(expression) STR_INNER(expression)
