include(${CMAKE_SOURCE_DIR}/test/cmake/TestProperties.cmake)

# Add a unit test to CTest.
#
# Required arguments:
#
#   SOURCES
#
#       A list of source files to compile and link into an executable.
#       One of these source files should have a main function.
#       Note that all unit tests are linked to the util and test_ libraries,
#       so their code can be used in the unit test.
#
# Optional arguments:
#
#   NAME
#
#       The target and test name will be unit_test_<NAME>.
#       Defaults to the file names of the source files joined together by _and_.
#
#   PROPERTIES
#
#       A list of properties to set for the test. The properties will be forwarded
#       onto the `set_test_properties` function from `TestProperties.cmake`. See
#       the documentation for `set_test_properties` for more details. Additionally:
#
#         - The TIMEOUT property defaults to 10 seconds; set to 0 to disable.
#         - The LABELS property has `unit_test` appended to it.
#
function(add_unit_test)
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
        message(FATAL_ERROR "add_unit_test given unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
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

    # Define the unit test.
    set(test_name unit_test_${ARG_NAME})
    add_executable(${test_name} ${ARG_SOURCES})
    target_compile_options(
        ${test_name}
        PRIVATE
        -DSKIP_RETURN_CODE=${SKIP_RETURN_CODE}
    )
    target_link_libraries(
        ${test_name}
        PUBLIC
        util
        test_
    )
    add_test(NAME ${test_name} COMMAND ${test_name})

    # Set the caller's properties on the test.
    set_test_properties(${test_name} ${ARG_PROPERTIES})

    # Add some default properties to the test.
    set_test_property_default(${test_name} TIMEOUT 10)
    set_test_property_default(${test_name} SKIP_RETURN_CODE ${SKIP_RETURN_CODE})
    append_test_property(${test_name} LABELS unit_test)
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
    set(ALL_UNIT_TESTS ${ALL_UNIT_TESTS} ${test_name} PARENT_SCOPE)
endfunction()


# Add multiple unit tests at once.
#
# Pass this function a list of source files and it will create one unit test
# for each, where the test will be called unit_test_<name> where the source
# file is dir/name.extension.
function(add_unit_tests)
    foreach(src ${ARGN})
        get_filename_component(target ${src} NAME_WE)
        add_unit_test(
            NAME ${target}
            SOURCES ${src}
        )
        # ALL_UNIT_TESTS get set in this scope so propagate it back up to module scope.
        set(ALL_UNIT_TESTS ${ALL_UNIT_TESTS} PARENT_SCOPE)
    endforeach()
endfunction()
