if(COVERAGE)
    include(cmake/Coverage.cmake)
endif()


###############################################################################
# Benchmarks
###############################################################################


include(cmake/Benchmark.cmake)

add_benchmarks(
    benchmark/parse.c
    benchmark/string.c
    # Add new benchmarks here :)
)


###############################################################################
# Unit tests
###############################################################################


include(cmake/UnitTest.cmake)

add_unit_tests(
    unit/cli.c
    unit/parse.c
    unit/string.c
    unit/timespec.c
    # Add new unit tests here :)
)


###############################################################################
# System tests
###############################################################################


include(cmake/SystemTest.cmake)

# This is an example system test that executes template/main.cpp (the template
# target) and checks that when passed the --version flag it prints the version
# to the standard output, which is verified by the PASS_REGULAR_EXPRESSION
# property.
#
# For more complex system tests you may wish to write a script that handles
# the details of the command and test validation.
add_system_test(
    COMMAND template --version
    PROPERTIES
        PASS_REGULAR_EXPRESSION "^${CMAKE_PROJECT_VERSION}\n$"
)

# This is an example failure test that executes template/main.cpp and passes
# if the executable exits with a non-zero exit code. The WILL_FAIL property
# is the only required one; the rest are just for show :)
add_system_test(
    COMMAND template --invalid-option
    PROPERTIES
        WILL_FAIL
        ENVIRONMENT COLOR=1 LOG_LEVEL=Debug
        LABELS template example
)

# Add new system tests here :)


###############################################################################
# Test coverage report
###############################################################################


if(COVERAGE)
    set(ALL_EXES ${ALL_UNIT_TESTS} ${ALL_SYSTEM_TEST_TARGETS})
    list(REMOVE_DUPLICATES ALL_EXES)
    add_coverage_report(
        TEST_TARGETS ${ALL_UNIT_TESTS} ${ALL_SYSTEM_TESTS}
        TEST_EXES ${ALL_EXES}
        FORMAT html
    )
endif()
