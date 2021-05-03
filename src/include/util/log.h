/// \file
/// Logging functions.
#pragma once

#include <stdio.h>

#include "util/color.h"
#include "util/env.h"


#ifdef __cplusplus
extern "C" {
#endif


/// The various severities of messages this logger supports.
enum log_level
{
    NONE,    ///< Disable all logging.
    FATAL,   ///< Log messages before terminating the application.
    ERROR,   ///< Log errors from the application.
    WARNING, ///< Log warnings to the user.
    SUCCESS, ///< Log successful operations.
    INFO,    ///< Log helpful information for the user.
    DEBUG,   ///< Log debug information for the developer.
};


/// Global log level. Defaults to ::INFO.
extern enum log_level global_log_level;


/// Return the name of the log level.
const char * log_level_to_string(enum log_level level);


/// Return the log level given the name.
/// Return `INT_MAX` if no level exists for the given name.
enum log_level string_to_log_level(const char * string);


/// Return the ANSI color code used to print messages at log \p level.
enum ANSI_color_codes log_level_to_color(enum log_level level);


/// Initialise the global log level.
///
/// This sets the application log level based on the `LOG_LEVEL` environment
/// variable. If the `LOG_LEVEL` environment variable does not exist then the
/// default log level is used.
void log_init();


/// Write a message to stdout or standard error.
#define log(level, ...)                                                                            \
    do                                                                                             \
    {                                                                                              \
        if (global_log_level >= level)                                                             \
        {                                                                                          \
            FILE * stream = (level <= WARNING) ? stderr : stdout;                                  \
            color_set(stream, log_level_to_color(level));                                          \
            fprintf(stream, "%s: ", log_level_to_string(level));                                   \
            fprintf(stream, "" __VA_ARGS__);                                                       \
            color_set(stream, WHITE);                                                              \
        }                                                                                          \
    } while (0)


/// Call log() with ::FATAL level.
#define fatal(...) log(FATAL, __VA_ARGS__)


/// Call log() with ::ERROR level.
#define error(...) log(ERROR, __VA_ARGS__)


/// Call log() with ::WARNING level.
#define warning(...) log(WARNING, __VA_ARGS__)


/// Call log() with ::SUCCESS level.
#define success(...) log(SUCCESS, __VA_ARGS__)


/// Call log() with ::INFO level.
#define info(...) log(INFO, __VA_ARGS__)


/// Call log() with ::DEBUG level.
#define debug(...) log(DEBUG, __VA_ARGS__)


#ifdef __cplusplus
} // extern "C"
#endif
