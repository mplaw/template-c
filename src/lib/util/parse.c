#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "util/util.h"


bool parse_bool(char const * string, bool * result)
{
    assert(string != NULL);
    assert(result != NULL);

    char const * false_values[] = {"false", "0", "off"};
    char const * true_values[] = {"true", "1", "on"};

    for (size_t i = 0; i < ARRAY_SIZE(false_values); ++i)
    {
        if (strcmp(string, false_values[i]) == 0)
        {
            *result = false;
            return true;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(true_values); ++i)
    {
        if (strcmp(string, true_values[i]) == 0)
        {
            *result = true;
            return true;
        }
    }

    return false;
}


bool parse_enum(char const * string, int * result, char const * enum_names[],
                size_t enum_names_size)
{
    assert(string != NULL);
    assert(result != NULL);
    assert(enum_names != NULL);

    for (size_t i = 0; i < enum_names_size; ++i)
    {
        if (enum_names[i] == NULL)
            continue;

        if (strcmp(string, enum_names[i]) == 0)
        {
            *result = i;
            return true;
        }
    }
    return false;
}


bool parse_uint32_t(char const * string, uint32_t * result)
{
    assert(string != NULL);
    assert(result != NULL);

    char * end = NULL;
    unsigned long long tmp = strtoull(string, &end, 10);
    if (end == string || tmp > (unsigned long long)UINT_MAX)
        return false;

    *result = (uint32_t)tmp;
    return true;
}


bool parse_int32_t(char const * string, int32_t * result)
{
    assert(string != NULL);
    assert(result != NULL);

    char * end = NULL;
    long long tmp = strtoll(string, &end, 10);
    if (end == string || tmp > (long long)INT_MAX || tmp < (long long)INT_MIN)
        return false;

    *result = (int32_t)tmp;
    return true;
}
