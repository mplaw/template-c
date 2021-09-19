#include "test/unit_test.h"


static void test_check_raises_sigint()
{
    CHECK_RAISES(raise(SIGINT), SIGINT);
}


static void test_check_raises_sigterm()
{
    CHECK_RAISES(raise(SIGTERM), SIGTERM);
}


int main()
{
    test_check_raises_sigint();
    test_check_raises_sigterm();
    success("All tests passed :-)");
    return 0;
}
