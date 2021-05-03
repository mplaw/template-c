#include <string.h>

#include "util/string.h"

#include "test/benchmark.h"


////////////////////////////////////////////////////////////////////////////////
// Slicing
////////////////////////////////////////////////////////////////////////////////


static struct string slice_output;
static struct string slice_input;
static size_t slice_from;
static size_t slice_to;


static void bench_string_copy_slice_func()
{
    string_copy_slice(&slice_output, &slice_input, slice_from, slice_to);
    CHECK(slice_output.size == (slice_to - slice_from));
    string_free(&slice_output);
}


static void bench_string_copy_slice()
{
    benchmark_init(__func__);

    slice_input = string_literal("0123456789");
    slice_from = 1;
    slice_to = 2;
    BENCH_WITH_NAME(bench_string_copy_slice_func, "12");

    slice_from = 1;
    slice_to = 8;
    BENCH_WITH_NAME(bench_string_copy_slice_func, "12345678");
}


static void bench_string_view_slice_func()
{
    string_view_slice(&slice_output, &slice_input, slice_from, slice_to);
}


static void bench_string_view_slice()
{
    benchmark_init(__func__);

    slice_input = string_literal("0123456789");
    slice_from = 1;
    slice_to = 2;
    BENCH_WITH_NAME(bench_string_view_slice_func, "12");

    slice_from = 1;
    slice_to = 8;
    BENCH_WITH_NAME(bench_string_view_slice_func, "12345678");
}


////////////////////////////////////////////////////////////////////////////////
// Splitting
////////////////////////////////////////////////////////////////////////////////


static struct string * split_output;
static struct string split_input;
static char split_pivot;
static size_t expected_out_size;


static void bench_string_copy_split_func()
{
    size_t out_size = string_copy_split(&split_output, &split_input, split_pivot);
    CHECK(out_size == expected_out_size);
    free(split_output);
}


static void bench_string_copy_split()
{
    benchmark_init(__func__);

    split_input = string_literal("0");
    split_pivot = '1';
    expected_out_size = 1;
    BENCH_WITH_NAME(bench_string_copy_split_func, split_input.data);

    split_input = string_literal("00000000");
    split_pivot = '1';
    expected_out_size = 1;
    BENCH_WITH_NAME(bench_string_copy_split_func, split_input.data);

    split_input = string_literal("0000000000000000");
    split_pivot = '1';
    expected_out_size = 1;
    BENCH_WITH_NAME(bench_string_copy_split_func, split_input.data);

    split_input = string_literal("010");
    split_pivot = '1';
    expected_out_size = 2;
    BENCH_WITH_NAME(bench_string_copy_split_func, split_input.data);

    split_input = string_literal("0101010");
    split_pivot = '1';
    expected_out_size = 4;
    BENCH_WITH_NAME(bench_string_copy_split_func, split_input.data);

    split_input = string_literal("010101010101010");
    split_pivot = '1';
    expected_out_size = 8;
    BENCH_WITH_NAME(bench_string_copy_split_func, split_input.data);
}


static void bench_string_view_split_func()
{
    size_t out_size = string_view_split(&split_output, &split_input, split_pivot);
    // It's reasonable to include the size check and call to free in the benchmark
    // because the fact that this function allocates means users have to validate
    // the allocation and call free, so including these overheads gives a more
    // accurate sense of the total application impact.
    CHECK(out_size == expected_out_size);
    free(split_output);
}


static void bench_string_view_split()
{
    benchmark_init(__func__);

    split_input = string_literal("0");
    split_pivot = '1';
    expected_out_size = 1;
    BENCH_WITH_NAME(bench_string_view_split_func, split_input.data);

    split_input = string_literal("00000000");
    split_pivot = '1';
    expected_out_size = 1;
    BENCH_WITH_NAME(bench_string_view_split_func, split_input.data);

    split_input = string_literal("0000000000000000");
    split_pivot = '1';
    expected_out_size = 1;
    BENCH_WITH_NAME(bench_string_view_split_func, split_input.data);

    split_input = string_literal("010");
    split_pivot = '1';
    expected_out_size = 2;
    BENCH_WITH_NAME(bench_string_view_split_func, split_input.data);

    split_input = string_literal("0101010");
    split_pivot = '1';
    expected_out_size = 4;
    BENCH_WITH_NAME(bench_string_view_split_func, split_input.data);

    split_input = string_literal("010101010101010");
    split_pivot = '1';
    expected_out_size = 8;
    BENCH_WITH_NAME(bench_string_view_split_func, split_input.data);
}


int main()
{
    bench_string_copy_slice();
    bench_string_view_slice();

    bench_string_copy_split();
    bench_string_view_split();
    return 0;
}
