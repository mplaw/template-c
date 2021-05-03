/// \file
/// Utilities for working with strings.
#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/// A string that also stores its size.
struct string
{
    char * data; ///< A pointer to the start of the string.
    size_t size; ///< The length of the string in bytes.
};


/// Alias `struct string` to something much shorter.
typedef struct string str_t;


////////////////////////////////////////////////////////////////////////////////
// Construction and destruction
////////////////////////////////////////////////////////////////////////////////


/// Allocate a new string and store the result in \p string.
///
/// If allocation fails the \p data member of \p string will
/// be set to \p NULL and the \p size member will be set to `0`.
///
/// When the string is no longer needed it should be passed
/// to the string_free() function.
void string_make(struct string * string, size_t size);


/// Create a copy of \p to_copy in \p string.
void string_copy(struct string * string, struct string * to_copy);


/// Create a copy of \p to_copy in \p string.
void string_copy_cstr(struct string * string, char const * to_copy);


/// Create a shallow copy of \p to_view in \p string.
void string_view(struct string * string, struct string * to_view);


/// Create a shallow copy of \p `[to_view,to_view+size)` in \p string.
void string_view_cstr_size(struct string * string, char * to_view, size_t size);


/// Create a shallow copy of \p to_view in \p string.
void string_view_cstr(struct string * string, char * to_view);


/// Create a shallow copy of the string literal \p \_\_s\_\_.
#define string_literal(__s__)                                                                      \
    (struct string)                                                                                \
    {                                                                                              \
        (char *)(__s__), sizeof(__s__) - 1                                                         \
    }


/// De-allocate memory from \p string.
void string_free(struct string * string);


////////////////////////////////////////////////////////////////////////////////
// Comparison
////////////////////////////////////////////////////////////////////////////////


/// Return true if \p string and \p other reference the same memory.
bool string_is_same(struct string const * string, struct string const * other);


/// Return true if the contents of \p string and \p other is the same.
bool string_is_equal(struct string const * string, struct string const * other);


/// Return true if the contents of \p string and \p other is the same.
bool string_is_equal_cstr(struct string const * string, char const * other);


////////////////////////////////////////////////////////////////////////////////
// Contains
////////////////////////////////////////////////////////////////////////////////


/// Return true if \p string starts with \p prefix.
bool string_starts_with_cstr(struct string const * string, char const * prefix);


/// Return true if \p string starts with \p prefix.
bool string_starts_with(struct string const * string, struct string const * prefix);


/// Return true if \p string ends with \p suffix.
bool string_end_with_cstr(struct string const * string, char const * suffix);


/// Return true if \p string ends with \p suffix.
bool string_ends_with(struct string const * string, struct string const * suffix);


////////////////////////////////////////////////////////////////////////////////
// Joining
////////////////////////////////////////////////////////////////////////////////


/// Concatenate `[strings, strings + count)` and store the result in \p result.
///
/// \note \p result must be deallocated by calling \rst :cppref:`~c/memory/free` \rst_end.
void string_join_array(struct string * result, struct string const * strings, size_t count);


/// Concatenate a variable number of ::string objects and store the result in \p result.
#define string_join(result, ...)                                                                   \
    do                                                                                             \
    {                                                                                              \
        struct string const args[] = {__VA_ARGS__};                                                \
        string_join_array((result), args, sizeof(args) / sizeof(args[0]));                         \
    } while (0)


////////////////////////////////////////////////////////////////////////////////
// Slicing
////////////////////////////////////////////////////////////////////////////////


/// Copy `to_slice[from, to)` into \p string.
///
/// \note \p string must be deallocated by calling ::string_free().
void string_copy_slice(struct string * string, struct string * to_slice, size_t from, size_t to);


/// View `to_slice[from, to)` from \p string.
void string_view_slice(struct string * string, struct string * to_slice, size_t from, size_t to);


////////////////////////////////////////////////////////////////////////////////
// Splitting
////////////////////////////////////////////////////////////////////////////////


/// Split the contents of \p to_split about \p pivot and store the pieces in \p output.
///
/// \note \p output must be deallocated by calling \rst :cppref:`~c/memory/free` \rst_end.
///
/// \param output   The resulting pieces from the split.
/// \param to_split The string to split into pieces.
/// \param pivot    The character to split about.
/// \return The number of strings stored in \p output.
size_t string_copy_split(struct string ** output, struct string const * to_split, char pivot);


/// Split the contents of \p to_split about \p pivot and store references to the pieces in \p output.
///
/// \note \p output must be deallocated by calling \rst :cppref:`~c/memory/free` \rst_end.
///
/// \param output   The resulting pieces from the split.
/// \param to_split The string to split into pieces.
/// \param pivot    The character to split about.
/// \return The number of strings stored in \p output.
size_t string_view_split(struct string ** output, struct string const * to_split, char pivot);



#ifdef __cplusplus
} // extern "C"
#endif
