/// \file
/// Utilities for working with timespecs.
#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>


#ifdef __cplusplus
extern "C" {
#endif


/// Return a timespec with zero seconds and zero nanoseconds.
struct timespec timespec_null();


/// Add \p rhs to \p lhs in-place.
void timespec_iadd(struct timespec * lhs, struct timespec const * rhs);


/// Subtract \p rhs from \p lhs in-place.
void timespec_isub(struct timespec * lhs, struct timespec const * rhs);


/// Multiply \p lhs by \p rhs in-place.
void timespec_imul(struct timespec * lhs, struct timespec * rhs);


/// Divide \p lhs by \p rhs in-place.
void timespec_idiv(struct timespec * lhs, long divisor);


/// Calculate the difference between \p start and \p stop and store the result in \p duration.
void timespec_duration(struct timespec * duration, struct timespec const * start,
                       struct timespec const * stop);


/// Return `true` if \p lhs is equal to \p rhs and return `false` otherwise.
bool timespec_is_equal(const struct timespec * lhs, const struct timespec * rhs);


/// Return `true` if \p lhs is less than \p rhs and return `false` otherwise.
bool timespec_is_less_than(const struct timespec * lhs, const struct timespec * rhs);


/// Compare two timespecs.
/// \return An integer representing the result of the comparison:
///     - -1 If \p lhs is less than \p rhs
///     -  0 If \p lhs is equal to \p rhs
///     -  1 If \p lhs is greather than \p rhs
int timespec_compare(void const * lhs, void const * rhs);


/// Convert \p t to a double in units of seconds.
double timespec_to_double_s(struct timespec * t);


/// Convert \p t to a double in units of nanoseconds.
double timespec_to_double_ns(struct timespec * t);


/// Convert \p d assuming its in units of seconds to a timespec.
struct timespec timespec_from_double_s(double d);


/// Convert \p d assuming its in units of nanoseconds to a timespec.
struct timespec timespec_from_double_ns(double d);


#ifdef __cplusplus
} // extern "C"
#endif
