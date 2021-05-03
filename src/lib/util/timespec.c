#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>


#include "util/timespec.h"


#define ASSERT_TIMESPEC_IS_OK(__TIMESPEC__)                                                        \
    assert((__TIMESPEC__) && (__TIMESPEC__)->tv_nsec < 1000000000LL)


struct timespec timespec_null()
{
    struct timespec result;
    result.tv_sec = 0;
    result.tv_nsec = 0;
    return result;
}


void timespec_iadd(struct timespec * lhs, struct timespec const * rhs)
{
    ASSERT_TIMESPEC_IS_OK(lhs);
    ASSERT_TIMESPEC_IS_OK(rhs);
    lhs->tv_sec += rhs->tv_sec;
    lhs->tv_nsec += rhs->tv_nsec;
    if (lhs->tv_nsec > 999999999L)
    {
        lhs->tv_sec += 1;
        lhs->tv_nsec -= 1000000000LL;
    }
}


void timespec_isub(struct timespec * lhs, struct timespec const * rhs)
{
    ASSERT_TIMESPEC_IS_OK(lhs);
    ASSERT_TIMESPEC_IS_OK(rhs);
    lhs->tv_sec -= rhs->tv_sec;
    lhs->tv_nsec -= rhs->tv_nsec;
    if (lhs->tv_nsec < 0)
    {
        lhs->tv_sec -= 1;
        lhs->tv_nsec += 1000000000LL;
    }
}


double timespec_to_double_s(struct timespec * t)
{
    return (double)t->tv_sec + (double)t->tv_nsec / 1000000000.0;
}


double timespec_to_double_ns(struct timespec * t)
{
    return (double)t->tv_sec * 1000000000.0 + (double)t->tv_nsec;
}


struct timespec timespec_from_double_s(double d)
{
    time_t sec = (time_t)d;
    // Round rather than truncate here to get a better chance at avoiding precision loss.
    long nsec = round(1000000000.0 * (d - (double)sec));
    return (struct timespec){sec, nsec};
}


struct timespec timespec_from_double_ns(double d)
{
    time_t sec = (time_t)(d / 1000000000.0);
    // Round rather than truncate here to get a better chance at avoiding precision loss.
    long nsec = round(d - (double)sec * 1000000000.0);
    while (nsec >= 1000000000)
    {
        sec += 1;
        nsec -= 1000000000;
    }
    return (struct timespec){sec, nsec};
}


void timespec_imul(struct timespec * lhs, struct timespec * rhs)
{
    ASSERT_TIMESPEC_IS_OK(lhs);
    ASSERT_TIMESPEC_IS_OK(rhs);
    double lhs_d = timespec_to_double_ns(lhs);
    double rhs_d = timespec_to_double_ns(rhs);
    *lhs = timespec_from_double_ns(lhs_d * rhs_d / 1000000000.0);
}


void timespec_idiv(struct timespec * lhs, long divisor)
{
    ASSERT_TIMESPEC_IS_OK(lhs);
    assert(divisor >= 0);
    lhs->tv_nsec /= divisor;
    // Carry second fraction part into nanoseconds.
    lhs->tv_nsec += ((lhs->tv_sec % divisor) * 1000000000LL / divisor);
    lhs->tv_sec /= divisor;
}


void timespec_duration(struct timespec * duration, struct timespec const * start,
                       struct timespec const * stop)
{
    ASSERT_TIMESPEC_IS_OK(start);
    ASSERT_TIMESPEC_IS_OK(stop);
    if (stop->tv_nsec < start->tv_nsec)
    {
        duration->tv_sec = stop->tv_sec - start->tv_sec - 1;
        duration->tv_nsec = (1000000000LL - start->tv_nsec) + stop->tv_nsec;
    }
    else
    {
        duration->tv_sec = stop->tv_sec - start->tv_sec;
        duration->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}


bool timespec_is_equal(const struct timespec * lhs, const struct timespec * rhs)
{
    ASSERT_TIMESPEC_IS_OK(lhs);
    ASSERT_TIMESPEC_IS_OK(rhs);
    return (lhs->tv_sec == rhs->tv_sec) && (lhs->tv_nsec == rhs->tv_nsec);
}


bool timespec_is_less_than(const struct timespec * lhs, const struct timespec * rhs)
{
    ASSERT_TIMESPEC_IS_OK(lhs);
    ASSERT_TIMESPEC_IS_OK(rhs);
    return (lhs->tv_sec < rhs->tv_sec) ||
           (lhs->tv_sec == rhs->tv_sec && lhs->tv_nsec < rhs->tv_nsec);
}


int timespec_compare(void const * lhs, void const * rhs)
{
    struct timespec const * _lhs = (struct timespec const *)lhs;
    struct timespec const * _rhs = (struct timespec const *)rhs;
    ASSERT_TIMESPEC_IS_OK(_lhs);
    ASSERT_TIMESPEC_IS_OK(_rhs);
    return timespec_is_less_than(_lhs, _rhs) ? -1 : timespec_is_less_than(_rhs, _lhs) ? 1 : 0;
}
