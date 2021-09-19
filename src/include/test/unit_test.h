/// \file
/// A very simple unit test framework.
#pragma once

#include <signal.h>
#include <stdlib.h>

#include "util/log.h"
#include "util/util.h"


/// Exit if \p \_\_EXPR\_\_ evaluates to `false`.
#define CHECK(__EXPR__)                                                                            \
    do                                                                                             \
    {                                                                                              \
        if (UNLIKELY(!(__EXPR__)))                                                                 \
        {                                                                                          \
            error("CHECK(%s) failed\n   in: %s\n   at: %s:%i\n", STR(__EXPR__), __func__,          \
                  __FILE__, __LINE__);                                                             \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)


/// @cond Doxygen_Suppress
void unit_test_signal_test_setup(int expected_signal);


int unit_test_signal_test_get_seen_signal();


void unit_test_signal_test_reset();
//! @endcond


/// Exit if \p expression does not raise \p \_\_signal\_\_.
/// \warning Do not execute this macro concurrently.
#define CHECK_RAISES(expression, __signal__)                                                       \
    do                                                                                             \
    {                                                                                              \
        unit_test_signal_test_setup(__signal__);                                                   \
        (void)(expression);                                                                        \
        CHECK(unit_test_signal_test_get_seen_signal() == (__signal__));                            \
        unit_test_signal_test_reset();                                                             \
    } while (0)
