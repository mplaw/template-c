/// \file
/// Command line interface.
#pragma once

#include "stdlib.h"


#ifdef __cplusplus
extern "C" {
#endif


/// Return codes for the `cli_*` family of functions.
enum cli_rc
{
    CLI_RC_OK = 0,                      ///< Success :)
    CLI_RC_INVALID_VALUE,               ///< An argument's value cannot be parsed.
    CLI_RC_UNSUPPORTED_ARGUMENT,        ///< An argument is unexpected.
    CLI_RC_MISSING_REQUIRED_ARGUMENTS,  ///< The CLI was provided fewer arguments than required.
    CLI_RC_MISSING_ARGUMENT_VALUE,      ///< An optional argument requires a value that was not provided.
};


/// The various actions the CLI library can take when it finds an argument.
enum cli_action
{
    PRINT_VERSION,  ///< Print the project's version to stdout and exit.
    PARSE_FLAG,     ///< Set a `bool` to `true` if the argument is found.
    PARSE_BOOL,     ///< Parse the value of the argument into a `bool`.
    PARSE_INT32,    ///< Parse the value of the argument into a `int32_t`.
    PARSE_UINT32,   ///< Parse the value of the argument into a `uint32_t`.
    PARSE_STRING,   ///< Parse the value of the argument into a `char *`.
};


/// A command line interface argument.
struct cli_arg
{
    /// The full name of the argument. Should start with `--` to be considered optional.
    /// Can be `NULL` if `short_name` is not `NULL`.
    char * long_name;
    /// The short name of the argument. Should start with `-` to be considered optional.
    /// Can be `NULL` if `long_name` is not `NULL`.
    char * short_name;
    /// The action that should be taken when parsing this argument.
    enum cli_action action;
    /// A pointer to the variable that should hold the value after the argument has been
    /// successfully parsed. Can be `NULL` if the `action` being taken isn't to write a
    /// value to variable.
    void * data;
    /// The size of the variable that `data` points to, in bytes.
    size_t data_size;
    /// A string that explains to the user what the argument does. Shown in help text.
    /// No description will be shown if `NULL`.
    char * description;
};


/// Describe the command line interface.
struct cli
{
    /// The name of the application. Shown in usage and help text. `argv[0]` will be printed if it's `NULL`.
    char * name;
    /// A description of the application. Shown in help text. Nothing will be printed if it's `NULL`.
    char * description;
    /// An array of `cli_args` that represent all the arguments the application supports.
    struct cli_arg * args;
    /// The length of the `args` array.
    size_t size;
};


/// Parse the arguments supplied to the application on the command line.
///
/// \param cli  The ::cli object that defines the supported arguments.
/// \param argc The length of the \p argv array.
/// \param argv An array of strings representing the arguments and their values.
void cli_parse(struct cli * cli, int argc, char * argv[]);


#ifdef __cplusplus
} // extern "C"
#endif
