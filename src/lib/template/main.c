#include <stdio.h>

#include "util/cli.h"
#include "util/log.h"
#include "util/util.h"


static const char version_string[] = PROJECT_VERSION;


int main(int argc, char * argv[])
{
    color_init();
    log_init();

    // Your code here :-)

    bool value;
    struct cli_arg args[] = {
        {"bool", NULL, PARSE_BOOL, &value, sizeof(value), "parse bool"},
        {"--version", "-v", PRINT_VERSION, (void *)version_string, sizeof(version_string),
         "print version and exit"},
        {"--log-level", "-l", PARSE_UINT32, &global_log_level, sizeof(global_log_level),
         "severity of log messages to print"},
    };
    struct cli cli = {argv[0], NULL, args, ARRAY_SIZE(args)};
    cli_parse(&cli, argc, argv);

    fatal("fatal message\n");
    error("error message\n");
    warning("warning message\n");
    info("info message: value=%i\n", value);
    debug("debug message\n");

    return CLI_RC_OK;
}
