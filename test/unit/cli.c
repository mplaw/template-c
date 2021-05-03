#include <stdint.h>

#include "util/cli.h"
#include "util/util.h"

#include "test/unit_test.h"


static void test_cli_parse()
{
    char * argv[] = {"test", "true", "42", "cheese gromit!", "-f", "true"};

    bool flag = false;
    bool boolean = false;
    uint32_t uint32 = 0;
    char * string = NULL;

    struct cli_arg args[] = {
        {"boolean", NULL, PARSE_BOOL, &boolean, sizeof(boolean), "parse bool"},
        {"integer", NULL, PARSE_UINT32, &uint32, sizeof(uint32), "parse integer"},
        {"string", NULL, PARSE_STRING, &string, sizeof(string), "parse string"},
        {"--flag", "-f", PARSE_FLAG, &flag, sizeof(flag), "parse flag"},
    };
    struct cli cli = {argv[0], NULL, args, ARRAY_SIZE(args)};
    cli_parse(&cli, ARRAY_SIZE(argv), argv);

    CHECK(boolean == true);
    CHECK(uint32 == 42);
    CHECK(strcmp(string, "cheese gromit!") == 0);
    CHECK(flag == true);
}


int main()
{
    test_cli_parse();
    success("All tests passed :-)");
    return 0;
}
