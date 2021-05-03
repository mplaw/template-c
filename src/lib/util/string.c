#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util/string.h"


////////////////////////////////////////////////////////////////////////////////
// Construction and destruction
////////////////////////////////////////////////////////////////////////////////


void string_make(struct string * string, size_t size)
{
    assert(string);

    string->data = (size > 0) ? (char *)malloc(size) : NULL;
    string->size = (string->data != NULL) ? size : 0;
}


void string_copy(struct string * string, struct string * to_copy)
{
    assert(string);
    assert(to_copy);

    string_make(string, to_copy->size);
    for (size_t i = 0; i < string->size; ++i)
        string->data[i] = to_copy->data[i];
}


void string_copy_cstr(struct string * string, char const * to_copy)
{
    assert(string);
    assert(to_copy);

    size_t size = 0;
    for (; to_copy[size] != '\0'; ++size)
        ;

    string_make(string, size);
    for (size_t i = 0; i < string->size; ++i)
        string->data[i] = to_copy[i];
}

void string_view(struct string * string, struct string * to_view)
{
    assert(string);
    assert(to_view);

    string->data = to_view->data;
    string->size = to_view->size;
}


void string_view_cstr_size(struct string * string, char * to_view, size_t size)
{
    assert(string);
    assert(to_view || size == 0);

    string->data = to_view;
    string->size = size;
}


void string_view_cstr(struct string * string, char * to_view)
{
    assert(string);
    assert(to_view);

    size_t size = 0;
    for (; to_view[size] != '\0'; ++size)
        ;

    string_view_cstr_size(string, to_view, size);
}


void string_free(struct string * string)
{
    assert(string);

    free(string->data);
    string->size = 0;
    string->data = NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Comparison
////////////////////////////////////////////////////////////////////////////////


bool string_is_same(struct string const * string, struct string const * other)
{
    assert(string);
    assert(other);

    return string->size == other->size && string->data == other->data;
}


bool string_is_equal(struct string const * string, struct string const * other)
{
    assert(string);
    assert(other);

    if (string->size != other->size)
        return false;

    for (size_t i = 0; i < string->size; ++i)
        if (string->data[i] != other->data[i])
            return false;

    return true;
}

bool string_is_equal_cstr(struct string const * string, char const * other)
{
    assert(string);
    assert(other);

    size_t i = 0;
    for (; i < string->size && other[i] != '\0'; ++i)
        if (string->data[i] != other[i])
            return false;

    return i == string->size && other[i] == '\0';
}


////////////////////////////////////////////////////////////////////////////////
// Contains
////////////////////////////////////////////////////////////////////////////////


bool string_starts_with_cstr(struct string const * string, char const * prefix)
{
    assert(string);
    assert(prefix);

    size_t i = 0;
    for (; i < string->size && prefix[i] != '\0'; ++i)
        if (string->data[i] != prefix[i])
            return false;

    return prefix[i] == '\0';
}


bool string_starts_with(struct string const * string, struct string const * prefix)
{
    assert(string);
    assert(prefix);

    if (prefix->size > string->size)
        return false;

    return string_starts_with_cstr(string, prefix->data);
}


bool string_end_with_cstr(struct string const * const string, char const * const suffix)
{
    const struct string temp = {(char*)suffix, strlen(suffix)};
    return string_ends_with(string, &temp);
}


bool string_ends_with(struct string const * const string, struct string const * const suffix)
{
    assert(string);
    assert(suffix);

    if (suffix->size > string->size)
        return false;

    size_t offset = string->size - suffix->size;
    for (size_t i = 0; i < suffix->size; ++i)
        if (string->data[offset + i] != suffix->data[i])
            return false;

    return true;
}


////////////////////////////////////////////////////////////////////////////////
// Searching
////////////////////////////////////////////////////////////////////////////////


size_t string_count_char(struct string const * string, char to_count)
{
    assert(string);

    size_t count = 0;
    for (size_t i = 0; i < string->size; ++i)
        count += string->data[i] == to_count;

    return count;
}


////////////////////////////////////////////////////////////////////////////////
// Joining
////////////////////////////////////////////////////////////////////////////////


void string_join_array(struct string * result, struct string const * strings, size_t count)
{
    assert(result);
    assert(strings || count == 0);

    size_t total_size = 0;
    for (size_t i = 0; i < count; ++i)
        total_size += strings[i].size;

    string_make(result, total_size);
    if (result->data == NULL)
        return;

    for (size_t i = 0, k = 0; i < count; ++i)
        for (size_t j = 0; j < strings[i].size; ++j, ++k)
            result->data[k] = strings[i].data[j];
}


////////////////////////////////////////////////////////////////////////////////
// Slicing
////////////////////////////////////////////////////////////////////////////////


void string_copy_slice(struct string * string, struct string * to_slice, size_t from, size_t to)
{
    assert(string);
    assert(to_slice);
    assert(to_slice->size >= to);
    assert(from <= to);

    string_make(string, to - from);
    for (size_t i = 0; i < string->size; ++i, ++from)
        string->data[i] = to_slice->data[from];
}


void string_view_slice(struct string * string, struct string * to_slice, size_t from, size_t to)
{
    assert(string);
    assert(to_slice);
    assert(to_slice->size >= to);
    assert(from <= to);

    string->data = to_slice->data + from;
    string->size = to - from;
}


size_t string_copy_split(struct string ** output, struct string const * to_split, char pivot)
{
    assert(output);
    assert(to_split);

    const size_t n_pivots = string_count_char(to_split, pivot);
    const size_t output_size = n_pivots + 1;

    // Only allocate once. Compute the total number of bytes to store n_pivot
    // string structures and the corresponding characters in the strings.
    size_t n_bytes = to_split->size + output_size * sizeof(struct string) - n_pivots;
    *output = (struct string*)malloc(n_bytes);
    if (*output == NULL)
        return 0;

    // Store the string structs in first followed by the split strings.
    struct string * string_data = *output;
    char * char_data = (char*)(string_data + output_size);

    // Loop through the original string and each time a character to split about
    // is found initialize a string object and copy in the characters.
    size_t last_boundary = 0;
    size_t j = 0;
    size_t k = 0;
    for (size_t i = 0; i < to_split->size; ++i)
    {
        if (to_split->data[i] != pivot)
            char_data[k++] = to_split->data[i];
        else
        {
            string_data[j].data = char_data + last_boundary;
            string_data[j].size = k - last_boundary;
            ++j;
            last_boundary = i;
        }
    }

    string_data[j].data = char_data + last_boundary;
    string_data[j].size = k - last_boundary;

    return output_size;
}


size_t string_view_split(struct string ** output, struct string const * to_split, char pivot)
{
    assert(output);
    assert(to_split);

    size_t number_of_pivots = string_count_char(to_split, pivot);
    size_t output_size = 1 + number_of_pivots;

    *output = malloc(sizeof(struct string) * output_size);
    if (*output == NULL)
        return 0;

    struct string * out_strings = *output;

    size_t last_boundary = 0;
    size_t j = 0;
    for (size_t i = 0; j < number_of_pivots; ++i)
    {
        if (to_split->data[i] == pivot)
        {
            out_strings[j].data = to_split->data + last_boundary;
            out_strings[j].size = i - last_boundary;
            ++j;
            last_boundary = i + 1;
            continue;
        }
    }

    out_strings[j].data = to_split->data + last_boundary;
    out_strings[j].size = to_split->size - last_boundary;

    return output_size;
}
