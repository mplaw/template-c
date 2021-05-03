#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "util/color.h"
#include "util/env.h"
#include "util/util.h"


bool color_enable = true;
bool color_force_enable = false;


void color_init()
{
    enum env_rc rc = env_get_bool("COLOR", &color_force_enable);
    switch (rc)
    {
        case ENV_RC_INVALID_VALUE:
            fprintf(stderr, "Error: Invalid value of COLOR environment variable.");
            exit(ENV_RC_INVALID_VALUE);
        case ENV_RC_NO_VARIABLE:
        case ENV_RC_OK:
            break;
    }
}


bool color_set(FILE * file, enum ANSI_color_codes color)
{
    if (!color_force_enable)
        if (!color_enable || !isatty(fileno(file)))
            return false;

    fprintf(file, "\033[0;%im", color);
    return true;
}
