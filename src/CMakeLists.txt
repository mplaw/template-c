# If coverage is enabled then instrument all code with coverage information.
if(COVERAGE)
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
endif()

# Util library
add_library(
    util
    STATIC
    lib/util/cli.c
    lib/util/color.c
    lib/util/env.c
    lib/util/log.c
    lib/util/parse.c
    lib/util/string.c
    lib/util/timespec.c
)
target_include_directories(util PUBLIC include)

# Test library
# Note that the target name 'test' is reserved by CTest, so here we've used `test_`
add_library(test_ STATIC lib/test/benchmark.c lib/test/unit_test.c)
target_include_directories(test_ PUBLIC include)
target_link_libraries(test_ PUBLIC util)
target_compile_definitions(test_ PRIVATE SKIP_RETURN_CODE=${SKIP_RETURN_CODE})

# Template library
add_executable(template lib/template/main.c)
target_link_libraries(template PUBLIC util)
target_compile_definitions(template PRIVATE PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

# Install the template target to the install tree.
install(TARGETS template)
