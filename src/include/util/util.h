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
