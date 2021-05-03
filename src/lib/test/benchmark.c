#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "test/benchmark.h"
#include "util/log.h"


// TODO: Different clock types.
// TODO: instruction barriers and all that jazz.
// TODO: Collect results in friendly format. Write to files and databases. JSON.


bool benchmark_overhead_is_set = false;
struct BenchmarkResults benchmark_overhead;


void benchmark_null()
{}


void benchmark_init(const char * name)
{
    log_init();

    // Print out a warning if running a debug build of the benchmarks.
#ifndef NDEBUG
    warning("This is a debug build. Use a release build for accurate benchmarks.\n");
#endif

    // Print out a name for the table of benchmarks.
    printf("\n %s\n", name);

    // Print out a header for the table of benchmarks.
    // clang-format off
    puts("--------------------------+-----------+-----------+-----------+-----------+-----------+-----------\n"
         " name                     |      mean |    stddev |       min |        1% |       99% |       max \n"
         "--------------------------+-----------+-----------+-----------+-----------+-----------+-----------");
    // clang-format on

    // Measure how long it takes to execute an empty function to get an
    // approximate idea of how much systematic error there will be.
    if (benchmark_overhead_is_set)
        return;

    benchmark_overhead = benchmark(benchmark_null);
    benchmark_overhead.name = "overhead";
    benchmark_overhead_is_set = true;
    benchmark_print_results(&benchmark_overhead);
}


void benchmark_print_results(struct BenchmarkResults * results)
{
    printf(" %-24s | %9lli | %9lli | %9lli | %9lli | %9lli | %9lli\n", results->name,
           1000000000LL * results->mean.tv_sec + results->mean.tv_nsec,
           1000000000LL * results->stddev.tv_sec + results->stddev.tv_nsec,
           1000000000LL * results->minimum.tv_sec + results->minimum.tv_nsec,
           // Only print the 1st and 99th percentile to save space.
           1000000000LL * results->percentiles[0].tv_sec + results->percentiles[0].tv_nsec,
           1000000000LL * results->percentiles[4].tv_sec + results->percentiles[4].tv_nsec,
           1000000000LL * results->maximum.tv_sec + results->maximum.tv_nsec);
}


struct timespec * benchmark_measure(void (*function)(void), size_t num_measurements,
                                    size_t warm_up_measurements, size_t iterations)
{
    assert(function != NULL);
    assert(num_measurements > 0 && iterations > 0);

    struct timespec * measurements = malloc(sizeof(struct timespec) * num_measurements);
    if (measurements == NULL)
        return NULL;

    // Touch every page of the measurements array so that the pages are acquired
    // by this process before we begin measuring. This may do more work than
    // necessary if using huge pages.
    // TODO: Check minimum page size on Windows.
    // TODO: Look-up page size programmatically.
    for (size_t i = 0; i < sizeof(struct timespec) * num_measurements; i += 4096)
        ((char *)measurements)[i] = 0;

    // We want to keep the measurement loop as small as possible to avoid polluting
    // the processor's instruction and data cache with code and data that isn't from
    // `function`. Some overhead is unavoidable, such as recording the time and storing
    // the result, however everything that can should be kept outside of this loop.
    // For example checking the return code of the timespec_get calls is moved to
    // after this loop.
    //
    // The first few iterations are treated as warm ups and their measurements are
    // discarded. The initial iteration will cause additional cache misses as the
    // processor will need to load all the code and data it needs to execute the
    // loop and `function`. Further warm-up iterations are useful for training the
    // branch predictor. Note if your architecture doesn't support these features
    // then you could manually set warm_up_iterations to zero.
    //
    // But honestly most of this is probably negligible because of the overhead of
    // the clock lol.
    int rc = TIME_UTC;
    struct timespec start, stop;
    for (unsigned m = 0; m < warm_up_measurements + num_measurements; ++m)
    {
        rc &= timespec_get(&start, TIME_UTC);
        for (unsigned i = 0; i < iterations; ++i)
            function();
        rc &= timespec_get(&stop, TIME_UTC);

        // Store the measurement.
        //
        // To try and make warm-up measurements as similar as possible to the real measurements
        // store the result regardless. For warm-up measurements it will always be stored to the
        // first element of the measurements array and overwritten by subsequent iterations.
        //
        // TODO: Consider storing the warm-up measurements as well.
        size_t index = (m >= warm_up_measurements) ? (m - warm_up_measurements) : 0;
        measurements[index].tv_sec = stop.tv_sec - start.tv_sec;
        measurements[index].tv_nsec = stop.tv_nsec - start.tv_nsec;
    }

    // Handle possible errors outside of the measurement loop to avoid polluting the cache.
    if (rc == 0)
    {
        error("Calling timespec_get failed. Benchmark is invalid. Exiting.");
        exit(1);
    }

    // Inside the core measurement loop we stored `stop - start` but didn't handle any wrap
    // -around cases, so handle these here. For very rapid measurements it's also possible
    // that `start > stop` and so `stop - start` can be negative, if for example the clock
    // is adjusted backwards by a greater duration than the duration of the measurement.
    for (size_t m = 0; m < num_measurements; ++m)
    {
        if (measurements[m].tv_sec < 0)
        {
            measurements[m] = timespec_null();
            debug("Measurement %zu is negative. This could be because the clock was adjusted "
                  "during the measurement. Consider increasing iterations in benchmark.\n",
                  m);
        }
        else if (measurements[m].tv_nsec < 0)
        {
            measurements[m].tv_nsec += 1000000000LL;
            measurements[m].tv_sec -= 1;
        }
    }

    return measurements;
}


struct BenchmarkResults benchmark_statistics(struct timespec * measurements,
                                             size_t num_measurements, size_t iterations)
{
    assert(num_measurements > 0 && iterations > 0);

    // Compute statistics.
    struct BenchmarkResults results;
    qsort(measurements, num_measurements, sizeof(struct timespec), timespec_compare);
    results.minimum = measurements[0];
    results.maximum = measurements[num_measurements - 1];

    // Compute the mean and standard deviation in a single pass.
    // This is not the most precise approach but for small numbers it seems stable.
    // Use doubles because timespecs don't have the capacity to store the sum squares.
    size_t num_useful_measurements = 0;
    double sum = 0;
    double stddev = 0;
    struct timespec nul = timespec_null();
    for (unsigned m = 0; m < num_measurements; ++m)
    {
        if (timespec_is_equal(measurements + m, &nul))
            continue;

        sum += timespec_to_double_s(measurements + m);

        double tmp = timespec_to_double_s(measurements + m);
        stddev += tmp * tmp;

        num_useful_measurements += 1;
    }

    if (num_useful_measurements == 0)
    {
        error("Number of useful measurements in benchmark is 0.\n");
        exit(1);
    }

    double sum_sq = (sum * sum) / num_useful_measurements;

    stddev -= sum_sq;
    stddev /= num_useful_measurements - 1;
    results.stddev = timespec_from_double_s(sqrt(stddev));

    results.mean = timespec_from_double_s(sum / num_useful_measurements);

    // Compute the percentiles.
    results.percentiles[0] = measurements[num_useful_measurements / 100];
    results.percentiles[1] = measurements[num_useful_measurements / 4];
    results.percentiles[2] = measurements[num_useful_measurements / 2];
    results.percentiles[3] = measurements[3 * num_useful_measurements / 4];
    if (num_useful_measurements < 100)
        results.percentiles[4] = measurements[(99 * num_useful_measurements) / 100];
    else
        results.percentiles[4] = measurements[(num_useful_measurements / 100) * 99];

    // Convert units to time per iteration.
    timespec_idiv(&results.minimum, iterations);
    timespec_idiv(&results.maximum, iterations);
    timespec_idiv(&results.mean, iterations);
    timespec_idiv(&results.stddev, iterations);
    for (size_t i = 0; i < ARRAY_SIZE(results.percentiles); ++i)
        timespec_idiv(results.percentiles + i, iterations);

    return results;
}


struct BenchmarkResults benchmark(void (*function)(void))
{
    const unsigned num_measurements = 1000;
    const unsigned iterations = 1000;

    struct timespec * measurements = benchmark_measure(function, num_measurements, 10, iterations);
    if (measurements == NULL)
    {
        error("Failed to allocate memory for %i measurements\n", num_measurements);
        exit(1);
    }

    struct BenchmarkResults results =
        benchmark_statistics(measurements, num_measurements, iterations);

    free(measurements);
    return results;
}
