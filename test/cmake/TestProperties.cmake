# This is a wrapper for set_tests_properties that also:
#
#   - handles lists neatly.
#   - validates arguments (unknown and duplicate arguments will cause an error).
#   - options are automatically set to true if provided and false if not.
#
# See https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#properties-on-tests
# for the list of supported arguments. Note that as properties are added this function may
# become out-of-date, in which case you can still use set_tests_properties.
function(set_test_properties target)
    # Early return to optimise case where no properties are specified.
    if(${ARGC} EQUAL 1)
        return()
    endif()

    # Define the function's arguments.
    set(options
        DISABLED
        RUN_SERIAL
        WILL_FAIL)
    set(one_value_kwargs
        COST
        MEASUREMENT
        PROCESSORS
        PROCESSOR_AFFINITY
        SKIP_RETURN_CODE
        TIMEOUT
        TIMEOUT_AFTER_MATCH
        WORKING_DIRECTORY)
    set(multi_value_kwargs
        ATTACHED_FILES
        ATTACHED_FILES_ON_FAIL
        LABELS
        DEPENDS
        ENVIRONMENT
        FAIL_REGULAR_EXPRESSION
        PASS_REGULAR_EXPRESSION
        SKIP_REGULAR_EXPRESSION
        FIXTURES_CLEANUP
        FIXTURES_REQUIRED
        FIXTURES_SETUP
        REQUIRED_FILES
        RESOURCE_GROUPS
        RESOURCE_LOCK)

    # Parse the function's arguments.
    cmake_parse_arguments(
        ARG
        "${options}"
        "${one_value_kwargs}"
        "${multi_value_kwargs}"
        ${ARGN})

    # Error if there's unexpected arguments.
    if(DEFINED ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "set_test_properties given unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    # Set the options in one go as they're guaranteed to have a value of true or false.
    set_tests_properties(
        ${target}
        PROPERTIES
            DISABLED ${ARG_DISABLED}
            RUN_SERIAL ${ARG_RUN_SERIAL}
            WILL_FAIL ${ARG_WILL_FAIL})

    # Use the set_property command to set properties because it handles lists properly.
    foreach(kwarg ${one_value_kwargs} ${multi_value_kwargs})
        if(DEFINED ARG_${kwarg})
            set(arg ARG_${kwarg})
            set_property(TEST ${target} PROPERTY ${kwarg} ${${arg}})
        endif()
    endforeach()

endfunction()


function(set_test_property_default target property default)
    get_test_property(${target} ${property} old_value)
    if("${old_value}" STREQUAL "NOTFOUND")
        set_property(TEST ${target} PROPERTY ${property} ${default})
    endif()
endfunction()


function(append_test_property target property)
    get_test_property(${target} ${property} old_list)
    if("${old_list}" STREQUAL "NOTFOUND")
        set(old_list ${ARGN})
    else()
        list(APPEND old_list ${ARGN})
    endif()
    set_property(TEST ${target} PROPERTY ${property} ${old_list})
endfunction()
