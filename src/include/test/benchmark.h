/// \file
/// A small and simple micro-benchmark library for benchmarking functions.
#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "test/unit_test.h"
#include "util/timespec.h"
#include "util/util.h"


#ifdef __cplusplus
extern "C" {
#endif


/// Store summary statistics for a benchmark.
struct BenchmarkResults
{
    const char * name;              ///< The name of the benchmark suite.
    struct timespec minimum;        ///< The smallest time it executed in.
    struct timespec maximum;        ///< The greatest time the function took.
    struct timespec mean;           ///< The average time of every function call.
    struct timespec stddev;         ///< The standard deviation of all measurements.
    struct timespec percentiles[5]; ///< 1%, 25%, 50% (median), 75%, 99% percentiles.
};


/// Print out a table header and overhead measurement for the benchmarks.
void benchmark_init(const char * name);


/// Write \p results to stdout.
void benchmark_print_results(struct BenchmarkResults * results);


/// Benchmark \p function.
///
/// This measures the time to execute \p function and returns
/// some summary statistics of the measurements.
struct BenchmarkResults benchmark(void (*function)(void));


/// Benchmark \p \_\_function\_\_ and use \p \_\_name\_\_ to identify it in the print out.
#define BENCH_WITH_NAME(__function__, __name__)                                                    \
    do                                                                                             \
    {                                                                                              \
        struct BenchmarkResults results = benchmark(__function__);                                 \
        results.name = __name__;                                                                   \
        benchmark_print_results(&results);                                                         \
    } while (0)


/// Benchmark \p \_\_function\_\_ and print out the results.
#define BENCH(__function__) BENCH_WITH_NAME(__function__, STR(__function__));


#ifdef __cplusplus
} // extern "C"
#endif
