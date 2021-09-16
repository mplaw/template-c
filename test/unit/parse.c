#include "util/parse.h"
#include "util/util.h"

#include "test/unit_test.h"


static void test_parse_bool()
{
    bool result = true;
    CHECK(parse_bool("0", &result));
    CHECK(result == false);

    CHECK(parse_bool("1", &result));
    CHECK(result == true);

    CHECK(parse_bool("off", &result));
    CHECK(result == false);

    CHECK(parse_bool("on", &result));
    CHECK(result == true);

    CHECK(parse_bool("false", &result));
    CHECK(result == false);

    CHECK(parse_bool("true", &result));
    CHECK(result == true);

    CHECK(parse_bool("0true", &result) == false);
    CHECK(result == true); // result should not have changed.
    CHECK(parse_bool("1false", &result) == false);
    CHECK(result == true); // result should not have changed.
}


static void test_parse_enum()
{
    enum test
    {
        FIRST,
        SECOND,
    };
    const char * test_names[] = {"First", "Second"};

    enum test result = -1;
    CHECK(parse_enum("First", &result, test_names, ARRAY_SIZE(test_names)) == true);
    CHECK(result == FIRST);

    result = -1;
    CHECK(parse_enum("Second", &result, test_names, ARRAY_SIZE(test_names)) == true);
    CHECK(result == SECOND);

    result = -1;
    CHECK(parse_enum("Third", &result, test_names, ARRAY_SIZE(test_names)) == false);
    CHECK(result == -1);
}


static void test_parse_uint32_t()
{
    uint32_t result = 1;
    CHECK(parse_uint32_t("0", &result));
    CHECK(result == 0);

    CHECK(parse_uint32_t("4294967295", &result));
    CHECK(result == 4294967295);

    result = 0;
    CHECK(parse_uint32_t("-1", &result) == false);
    CHECK(result == 0); // result should not have changed.
    CHECK(parse_uint32_t("4294967296", &result) == false);
    CHECK(result == 0); // result should not have changed.
}


static void test_parse_int32_t()
{
    int32_t result = 1;
    CHECK(parse_int32_t("0", &result));
    CHECK(result == 0);

    CHECK(parse_int32_t("2147483647", &result));
    CHECK(result == 2147483647);

    CHECK(parse_int32_t("-2147483648", &result));
    CHECK(result == -2147483648);

    result = 0;
    CHECK(parse_int32_t("-1", &result) == true);
    CHECK(result == -1);

    result = 0;
    CHECK(parse_int32_t("2147483648", &result) == false);
    CHECK(result == 0); // result should not have changed.
    CHECK(parse_int32_t("-2147483649", &result) == false);
    CHECK(result == 0);
}


int main()
{
    test_parse_bool();
    test_parse_enum();
    test_parse_uint32_t();
    test_parse_int32_t();
    success("All tests passed :-)");
    return 0;
}
