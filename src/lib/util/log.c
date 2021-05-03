#include <assert.h>
#include <limits.h>
#include <string.h>

#include "util/env.h"
#include "util/log.h"
#include "util/util.h"


enum log_level global_log_level = INFO;


const char * log_names[] = {
    "None", "Fatal", "Error", "Warning", "Success", "Info", "Debug",
};


static_assert(DEBUG + 1 == ARRAY_SIZE(log_names), "log_level and log_names do not match.");


void log_init()
{
    color_init();

    enum env_rc rc = env_get_enum("LOG_LEVEL", &global_log_level, log_names, ARRAY_SIZE(log_names));
    if (rc == ENV_RC_INVALID_VALUE)
    {
        error("Environment variable LOG_LEVEL has an invalid value: %i\n", (int)global_log_level);
        exit(1);
    }
}


const char * log_level_to_string(enum log_level level)
{
    assert(level <= DEBUG);
    return log_names[level];
}


enum log_level string_to_log_level(const char * string)
{
    assert(string != NULL);

    for (int i = 0; i <= DEBUG; ++i)
        if (strcmp(string, log_names[i]) == 0)
            return i;

    return INT_MAX;
}


enum ANSI_color_codes log_level_to_color(enum log_level level)
{
    switch (level)
    {
        case NONE:
            return WHITE;
        case FATAL:
            return PURPLE;
        case ERROR:
            return RED;
        case WARNING:
            return YELLOW;
        case SUCCESS:
            return GREEN;
        case INFO:
            return WHITE;
        case DEBUG:
            return LIGHT_GRAY;
        default:
            assert(false);
            return WHITE;
    }
}
