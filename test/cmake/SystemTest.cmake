include(${CMAKE_SOURCE_DIR}/test/cmake/TestProperties.cmake)


# Add a system test to CTest.
#
# Required arguments:
#
#   COMMAND
#
#       The command line to execute. If the process returns 0 then the test
#       has passed and if it returns a non-zero value then the test failed.
#       The opposite is true if the `WILL_FAIL` property is set to `true`.
#
# Optional arguments:
#
#   NAME
#
#       The test name. Defaults to a stringified form of COMMAND where double
#       dashes have been removed and spaces + single dashes have been replaced
#       by underscores. For example the command `template --invalid-option` will
#       generate a name of template_invalid_option.
#
#   PROPERTIES
#
#       A list of properties to set for the test. The properties will be forwarded
#       onto the `set_test_properties` function from `TestProperties.cmake`. See
#       the documentation for `set_test_properties` for more details. Additionally:
#
#         - The TIMEOUT property defaults to 60 seconds; set to 0 to disable.
#         - The LABELS property has `system_test` appended to it. If the WILL_FAIL
#           property is set then the `failure_test` label is also appended.
#
function(add_system_test)
    # Parse the function's arguments.
    set(options)
    set(one_value_kwargs NAME)
    set(multi_value_kwargs COMMAND PROPERTIES)
    cmake_parse_arguments(
        ARG
        "${options}"
        "${one_value_kwargs}"
        "${multi_value_kwargs}"
        ${ARGN}
    )

    # Validate the function's arguments.
    if(DEFINED ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "add_system_test given unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT DEFINED ARG_COMMAND)
        message(FATAL_ERROR "COMMAND must be defined")
    endif()

    # Build a test name from the command if NAME is not provided.
    if(NOT DEFINED ARG_NAME)
        string(REPLACE ";" "_" ARG_NAME "${ARG_COMMAND}")
        string(REPLACE "--" "" ARG_NAME "${ARG_NAME}")
        string(REPLACE "-" "_" ARG_NAME "${ARG_NAME}")
    endif()

    # Define the system test.
    set(test_name system_test_${ARG_NAME})
    add_test(NAME ${test_name} COMMAND ${ARG_COMMAND})

    # Set the caller's properties on the test.
    set_test_properties(${test_name} ${ARG_PROPERTIES})

    # Add some default properties to the test.
    set_test_property_default(${test_name} TIMEOUT 60)
    set(new_labels system_test)
    get_test_property(${test_name} WILL_FAIL is_failure_test)
    if(${is_failure_test})
        list(APPEND new_labels failure_test)
    endif()
    append_test_property(${test_name} LABELS ${new_labels})
    if(COVERAGE_PROFRAW_DIR)
        # If we're generating a coverage report then use a unique name for
        # the output profile file to avoid overwriting the output files
        # from other tests. Target is guaranteed to be unique by cmake.
        append_test_property(
            ${test_name}
            ENVIRONMENT
            LLVM_PROFILE_FILE=${COVERAGE_PROFRAW_DIR}/${test_name}.profraw
        )
    endif()

    # Create a list of all the test targets we've created.
    set(ALL_SYSTEM_TESTS ${ALL_SYSTEM_TESTS} ${test_name} PARENT_SCOPE)

    # Also store the list of targets being used by command.
    foreach(value ${COMMAND})
        if(TARGET ${value})
            list(APPEND all_targets ${value})
        endif()
    endforeach()
    set(ALL_SYSTEM_TEST_TARGETS ${ALL_SYSTEM_TEST_EXES} ${all_targets} PARENT_SCOPE)
endfunction()
