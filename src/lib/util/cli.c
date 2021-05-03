#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/cli.h"
#include "util/log.h"
#include "util/parse.h"


static char * cli_action_name(enum cli_action action)
{
    switch (action)
    {
        case PRINT_VERSION:
            return "version";
        case PARSE_FLAG:
            return "flag";
        case PARSE_BOOL:
            return "bool";
        case PARSE_UINT32:
            return "uint32";
        case PARSE_STRING:
            return "string";
        default:
            assert(false);
    }
}

static bool cli_arg_is_positional(struct cli_arg const * arg)
{
    return !((arg->short_name != NULL && arg->short_name[0] == '-') ||
             (arg->long_name != NULL && arg->long_name[0] == '-'));
}


static enum cli_rc parse_arg(struct cli_arg * arg, const char * string)
{
    switch (arg->action)
    {
        case PRINT_VERSION:
            puts((char *)arg->data);
            exit(0);
        case PARSE_FLAG:
            *(bool *)arg->data = true;
            break;
        case PARSE_BOOL:
            return parse_bool(string, (bool *)arg->data) ? CLI_RC_OK : CLI_RC_INVALID_VALUE;
        case PARSE_UINT32:
            return parse_uint32_t(string, (unsigned *)arg->data) ? CLI_RC_OK : CLI_RC_INVALID_VALUE;
        case PARSE_STRING:
            *((char **)arg->data) = (char *)string;
            break;
        default:
            return CLI_RC_UNSUPPORTED_ARGUMENT;
    }
    return CLI_RC_OK;
}


static bool is_match(char const * string, struct cli_arg const * arg)
{
    assert(string);

    if (string[0] == '-')
    {
        // Long-name optional argument: --option
        if (string[1] == '-')
            return arg->long_name != NULL && strcmp(string + 2, arg->long_name + 2) == 0;

        // Short-name optional argument: -o
        return arg->short_name != NULL && strcmp(string + 1, arg->short_name + 1) == 0;
    }

    // Positional argument: option
    return arg->long_name != NULL && strcmp(string, arg->long_name) == 0;
}


static void print_usage(struct cli * cli)
{
    printf("Usage: %s", cli->name);
    // Print required arguments.
    // The required arguments are the most important ones so only print them.
    // Users can see the optional arguments with the --help option.
    for (struct cli_arg * arg = cli->args; arg != cli->args + cli->size; ++arg)
        if (cli_arg_is_positional(arg))
            printf(" <%s>", arg->long_name);

    printf("\n");
}


static void print_help_and_exit(struct cli * cli)
{
    print_usage(cli);

    if (cli->description != NULL)
        printf("Description:\n%s\n", cli->description);

    printf("\nRequired arguments:\n");
    for (struct cli_arg * arg = cli->args; arg != cli->args + cli->size; ++arg)
        if (cli_arg_is_positional(arg))
            printf("\t<%s>\n\t\t%s\n", arg->long_name, arg->description);

    printf("\nOptional arguments:\n");
    for (struct cli_arg * arg = cli->args; arg != cli->args + cli->size; ++arg)
    {
        if (cli_arg_is_positional(arg))
            continue;
        if (arg->short_name != NULL && arg->long_name != NULL && arg->data != NULL)
            printf("\t%s, %s <%s>\n\t\t%s\n", arg->short_name, arg->long_name, arg->long_name + 2,
                   arg->description);
        else if (arg->short_name != NULL && arg->long_name != NULL)
            printf("\t%s, %s\n\t\t%s\n", arg->short_name, arg->long_name, arg->description);
        else if (arg->short_name != NULL)
            printf("\t%s\n\t\t%s\n", arg->short_name, arg->description);
        else if (arg->long_name != NULL)
            printf("\t%s\n\t\t%s\n", arg->long_name, arg->description);
        else
            assert(false);
    }
    exit(0);
}


void cli_parse(struct cli * cli, int argc, char * argv[])
{
    if (cli->name == NULL)
        cli->name = argv[0];

    size_t consumed_required_arguments = 0;
    size_t number_of_required_args = 0;
    for (size_t i = 0; i < cli->size; ++i)
        if (cli_arg_is_positional(cli->args + i))
            number_of_required_args += 1;

    for (int i = 1; i < argc; ++i)
    {
        char const * arg = argv[i];

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)
            print_help_and_exit(cli);

        if (strcmp(arg, "--") == 0)
            return;

        bool parsed = false;
        size_t seen_positionals = 0;
        for (size_t j = 0; j < cli->size; ++j)
        {
            struct cli_arg * cli_arg = cli->args + j;

            const bool is_positional = cli_arg_is_positional(cli_arg);
            if (is_positional)
            {
                // Skip this CLI member if the argument is non-positional.
                if (arg[0] == '-')
                    continue;

                // Skip positional arguments we've already parsed.
                if (seen_positionals < consumed_required_arguments)
                {
                    seen_positionals += 1;
                    continue;
                }
            }
            else
            {
                if (!is_match(arg, cli_arg))
                    continue;

                if (i == argc)
                {
                    error("Argument %s requires a value", cli_arg->long_name);
                    exit(CLI_RC_MISSING_ARGUMENT_VALUE);
                }
            }

            char const * val = is_positional ? arg : argv[i + 1];
            switch (parse_arg(cli_arg, val))
            {
                case CLI_RC_OK:
                    break;
                case CLI_RC_INVALID_VALUE:
                    error("Argument %zu has an invalid value: '%s'. Expected type: %s\n", j,
                          val, cli_action_name(cli_arg->action));
                    exit(CLI_RC_INVALID_VALUE);
                case CLI_RC_UNSUPPORTED_ARGUMENT:
                    error("Argument %zu is unsupported: unknown action type: %s\n", j,
                          cli_action_name(cli_arg->action));
                    exit(CLI_RC_UNSUPPORTED_ARGUMENT);
                case CLI_RC_MISSING_ARGUMENT_VALUE:
                case CLI_RC_MISSING_REQUIRED_ARGUMENTS:
                    assert(false); // impossible
            }

            parsed = true;
            consumed_required_arguments += is_positional;
            i += !is_positional;
            break;
        }

        if (parsed)
            continue;

        error("Argument %i is unsupported: %s\n", i, arg);
        print_usage(cli);
        exit(CLI_RC_UNSUPPORTED_ARGUMENT);
    }

    if (consumed_required_arguments != number_of_required_args)
    {
        error("Missing required arguments. Got %zu, expected %zu.\n", consumed_required_arguments,
              number_of_required_args);
        print_usage(cli);
        exit(CLI_RC_MISSING_REQUIRED_ARGUMENTS);
    }
}
