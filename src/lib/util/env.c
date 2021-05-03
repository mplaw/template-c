// The `getenv` function is deprecated on Windows because multiple calls to
// it invalidate the return value of prior calls. The recommended solution
// is to use the `dupenv_s` or `getenv_s` functions. The `dupenv_s` function
// is Windows only and dynamically allocates memory. The `getenv_s` function
// is an optional feature of C11 that doesn't seem to be widely implemented
// (Godbolt's clang 12.0.0 and gcc 10.3 both don't recognise the function).
//
// As this code copies the result of `getenv` to a buffer immediately it's
// OK although still not thread-safe. Note that this could be made more
// efficient as getenv does a copy internally that is unnecessary given
// that we're copying it as well, however at the moment the portability
// of `getenv` (which is standard C) seems like a worthwhile trade-off.
#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util/env.h"
#include "util/parse.h"
#include "util/util.h"


enum env_rc env_get(char const * name, size_t * len, char * out, size_t out_size)
{
    assert(name != NULL);

#ifdef UNIVERSAL_WINDOWS_PLATFORM
    // The Universal Windows Platform doesn't support environment variables.
    return ENV_RC_NO_VARIABLE;
#else
    char const * var = getenv(name);
    if (var == NULL)
        return ENV_RC_NO_VARIABLE;

    if (out == NULL || out_size == 0)
        return ENV_RC_OK;

    size_t i = 0;
    for (; i < (out_size - 1) && var[i] != '\0'; ++i)
        out[i] = var[i];
    out[i] = '\0';

    if (len != NULL)
        *len = i;

    if (var[i] != '\0')
        return ENV_RC_INVALID_VALUE;

    return ENV_RC_OK;
#endif
}


bool env_has(char const * name)
{
    return env_get(name, NULL, NULL, 0) == ENV_RC_OK;
}


enum env_rc env_get_bool(char const * name, bool * result)
{
    assert(result != NULL);

    // Only store enough bytes to store the longest false/true value: `false`.
    size_t len;
    char buffer[6];
    enum env_rc rc = env_get(name, &len, buffer, ARRAY_SIZE(buffer));
    if (rc != ENV_RC_OK)
        return rc;

    // Convert the environment variable value to lower-case.
    for (size_t i = 0; i < len; ++i)
        buffer[i] = tolower(buffer[i]);

    bool success = parse_bool(buffer, result);
    if (!success)
        return ENV_RC_INVALID_VALUE;

    return ENV_RC_OK;
}


enum env_rc env_get_enum(char const * name, int * result, char const * enum_names[],
                         size_t enum_names_size)
{
    assert(result != NULL);

    char const * var = getenv(name);
    if (var == NULL)
        return ENV_RC_NO_VARIABLE;

    bool success = parse_enum(var, result, enum_names, enum_names_size);
    if (!success)
        return ENV_RC_INVALID_VALUE;

    return ENV_RC_OK;
}
