include(${CMAKE_SOURCE_DIR}/test/cmake/TestProperties.cmake)


# Add a benchmark to CTest.
#
# Required arguments:
#
#   SOURCES
#
#       A list of source files to compile and link into an executable.
#       One of these source files should have a main function.
#       Note that all benchmarks are linked to the util and test_ libraries,
#       so their code can be used in the benchmark.
#
# Optional arguments:
#
#   NAME
#
#       The target and benchmark name will be benchmark_<NAME>.
#       Defaults to the file names of the source files joined together by _and_.
#
#   PROPERTIES
#
#       A list of properties to set for the benchmark. The properties will be forwarded
#       onto the `set_test_properties` function from `TestProperties.cmake`. See
#       the documentation for `set_test_properties` for more details. Additionally:
#
#         - The TIMEOUT property defaults to 60 seconds; set to 0 to disable.
#         - The LABELS property has `benchmark` appended to it.
#         - The PROCESSORS property defaults to 1.
#         - The PROCESSOR_AFFINITY property defaults to true.
#         - The RUN_SERIAL property defaults to true.
#
function(add_benchmark)
    # Parse the function's arguments.
    set(options)
    set(one_value_kwargs NAME)
    set(multi_value_kwargs SOURCES PROPERTIES)
    cmake_parse_arguments(
        ARG
        "${options}"
        "${one_value_kwargs}"
        "${multi_value_kwargs}"
        ${ARGN}
    )

    # Validate the function's arguments.
    if(DEFINED ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "add_benchmark given unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT DEFINED ARG_SOURCES)
        message(FATAL_ERROR "SOURCES must be defined")
    endif()

    # Build a test name from the command if NAME is not provided.
    if(NOT DEFINED ARG_NAME)
        foreach(src ${SOURCES})
            get_filename_component(file_name ${src} NAME_WE)
            if(NOT DEFINED ARG_NAME)
                set(ARG_NAME "${file_name}")
            else()
                set(ARG_NAME "${ARG_NAME}_and_${file_name}")
            endif()
        endforeach()
    endif()

    # Define the benchmark.
    set(benchmark_name benchmark_${ARG_NAME})
    add_executable(${benchmark_name} ${ARG_SOURCES})
    target_link_libraries(
        ${benchmark_name}
        PUBLIC
        test_
        util
    )
    add_test(NAME ${benchmark_name} COMMAND ${benchmark_name})

    # Set some default properties.
    set_tests_properties(
        ${benchmark_name}
        PROPERTIES
            PROCESSORS 1
            PROCESSOR_AFFINITY true
            RUN_SERIAL true
            TIMEOUT 60
    )

    # Set the caller's properties on the test.
    set_test_properties(${benchmark_name} ${ARG_PROPERTIES})

    append_test_property(${benchmark_name} LABELS benchmark)
endfunction()

# Add multiple benchmark targets at once.
function(add_benchmarks)
    foreach(src ${ARGN})
        get_filename_component(target ${src} NAME_WE)
        add_benchmark(NAME ${target} SOURCES ${src})
    endforeach()
endfunction()
