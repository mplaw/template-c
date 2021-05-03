#include "util/timespec.h"

#include "test/unit_test.h"


static void test_timespec_is_equal()
{
    {
        struct timespec lhs = {1, 2};
        struct timespec rhs = {1, 2};
        CHECK(timespec_is_equal(&lhs, &rhs));
    }
    {
        struct timespec lhs = {1, 2};
        struct timespec rhs = {1, 3};
        CHECK(!timespec_is_equal(&lhs, &rhs));
    }
}


static void test_timespec_is_less_than()
{
    {
        struct timespec lhs = {1, 1};
        struct timespec rhs = {1, 2};
        CHECK(timespec_is_less_than(&lhs, &rhs));
    }
    {
        struct timespec lhs = {0, 2};
        struct timespec rhs = {1, 2};
        CHECK(timespec_is_less_than(&lhs, &rhs));
    }
    {
        struct timespec lhs = {0, 999999999};
        struct timespec rhs = {1, 0};
        CHECK(timespec_is_less_than(&lhs, &rhs));
    }
}


static void test_timespec_iadd()
{
    {
        struct timespec lhs = {1, 2};
        struct timespec rhs = {3, 4};
        timespec_iadd(&lhs, &rhs);
        CHECK(lhs.tv_sec == 4);
        CHECK(lhs.tv_nsec == 6);
    }
    // Nano-seconds should overflow into seconds.
    {
        struct timespec lhs = {0, 1};
        struct timespec rhs = {0, 999999999};
        timespec_iadd(&lhs, &rhs);
        CHECK(lhs.tv_sec == 1);
        CHECK(lhs.tv_nsec == 0);
    }
}


static void test_timespec_imul()
{
    {
        struct timespec lhs = {1, 0};
        struct timespec rhs = {0, 0};
        timespec_imul(&lhs, &rhs);
        CHECK(lhs.tv_sec == 0);
        CHECK(lhs.tv_nsec == 0);
    }
    {
        struct timespec lhs = {1, 2};
        struct timespec rhs = {2, 0};
        timespec_imul(&lhs, &rhs);
        CHECK(lhs.tv_sec == 2);
        CHECK(lhs.tv_nsec == 4);
    }
}


static void test_timespec_idiv()
{
    {
        struct timespec lhs = {1, 0};
        timespec_idiv(&lhs, 3);
        CHECK(lhs.tv_sec == 0);
        CHECK(lhs.tv_nsec == 333333333);
    }
}


static void test_timespec_compare()
{
    {
        struct timespec lhs = {1, 0};
        struct timespec rhs = {1, 0};
        CHECK(timespec_compare(&lhs, &rhs) == 0);
    }
    {
        struct timespec lhs = {0, 0};
        struct timespec rhs = {1, 0};
        CHECK(timespec_compare(&lhs, &rhs) == -1);
    }
    {
        struct timespec lhs = {1, 0};
        struct timespec rhs = {0, 0};
        CHECK(timespec_compare(&lhs, &rhs) == 1);
    }
}


int main()
{
    test_timespec_is_equal();
    test_timespec_is_less_than();
    test_timespec_iadd();
    test_timespec_imul();
    test_timespec_idiv();
    test_timespec_compare();
    success("All tests passed :-)");
    return 0;
}
