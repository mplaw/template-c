#include "util/parse.h"

#include "test/benchmark.h"
#include "test/unit_test.h"


// Use the same variables for all benchmarks so that the memory locations
// for the variables are the same for each benchmark.
static char * string;
static bool result;
static bool expected;


static void bench_parse_bool_func()
{
    bool rc = parse_bool(string, &result);
    CHECK(rc == true);
    CHECK(result == expected);
}


static void bench_parse_bool()
{
    benchmark_init(__func__);

    string = "true";
    expected = true;
    BENCH_WITH_NAME(bench_parse_bool_func, "parse_bool(true)");

    string = "1";
    expected = true;
    BENCH_WITH_NAME(bench_parse_bool_func, "parse_bool(1)");

    string = "on";
    expected = true;
    BENCH_WITH_NAME(bench_parse_bool_func, "parse_bool(on)");

    string = "false";
    expected = false;
    BENCH_WITH_NAME(bench_parse_bool_func, "parse_bool(false)");

    string = "0";
    expected = false;
    BENCH_WITH_NAME(bench_parse_bool_func, "parse_bool(0)");

    string = "off";
    expected = false;
    BENCH_WITH_NAME(bench_parse_bool_func, "parse_bool(off)");
}


int main()
{
    bench_parse_bool();
    return 0;
}
