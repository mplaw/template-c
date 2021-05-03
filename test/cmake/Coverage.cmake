# Coverage requires these tools.
find_program(LLVM_COV llvm-cov REQUIRED)
find_program(LLVM_PROFDATA llvm-profdata REQUIRED)

# Store the coverage output in a single directory: <build>/coverage.
set(COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
# Store the profraw files in a single directory, <coverage>/profraw,
# to not pollute the current working directory with .profraw files.
set(COVERAGE_PROFRAW_DIR ${COVERAGE_DIR}/profraw)

# Create a build target that generates a test coverage report.
#
# The target is called `coverage_<FORMAT>` where `FORMAT` defaults to html.
# The target will:
#
#   1. Run CTest to generate .profraw files.
#   2. Merge the .profraw files together.
#   3. Create a coverage report for the `TEST_TARGETS`.
#
# Required arguments:
#
#   TEST_TARGETS (list)
#       The test targets that should be instrumented for coverage.
#       These tests will be executed as part of generating coverage data.
#
#   TEST_EXES (list)
#       The executables used when running CTest.
#
# Optional arguments:
#
#   FORMAT (string) [=html]
#       The format for the coverage report. One of: html, text.
#
function(add_coverage_report)
    # Define optional function arguments.
    set(options "")
    set(one_value_kwargs FORMAT)
    set(multi_value_kwargs TEST_TARGETS TEST_EXES)

    # Parse function arguments.
    cmake_parse_arguments(
        ARG
        "${options}"
        "${one_value_kwargs}"
        "${multi_value_kwargs}"
        ${ARGN}
    )

    # Validate arguments.
    if(DEFINED ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "add_coverage_report given unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT DEFINED ARG_TEST_TARGETS)
        message(FATAL_ERROR "TEST_TARGETS argument is required")
    endif()

    # Use html as the default output format.
    if(NOT DEFINED ARG_FORMAT)
        set(ARG_FORMAT html)
    endif()

    # For each test use a unique name for the generated profraw file so that
    # that tests don't overwrite each other's coverage output. Store all the
    # names in `test_profraw_files` so we can use them as dependencies. Note
    # that the add_*_test functions set the LLVM_PROFILE_FILE environment
    # variable so that these unique names are respected.
    set(test_profraw_files )
    foreach(test ${ARG_TEST_TARGETS})
        set(test_profraw ${COVERAGE_PROFRAW_DIR}/${test}.profraw)
        list(APPEND test_profraw_files ${test_profraw})
    endforeach()

    # Run CTest to generate all the profraw files.
    # Only execute the tests for which we have been passed targets.
    list(JOIN ARG_TEST_TARGETS "|" TEST_REGEX)
    add_custom_command(
        OUTPUT ${test_profraw_files}
        # Remove any existing .profraw files first to stop them being used.
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${COVERAGE_PROFRAW_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} -R \"${TEST_REGEX}\"
        DEPENDS ${ARG_TEST_EXES}
    )

    # Merge the generated profraw files together into a single profraw file,
    # merged.profraw.
    set(MERGED_PROFDATA ${COVERAGE_PROFRAW_DIR}/merged.profraw)
    add_custom_command(
        OUTPUT ${MERGED_PROFDATA}
        # Wipe the merged file to force llvm-profdata to re-generate it.
        COMMAND ${CMAKE_COMMAND} -E rm -f ${MERGED_PROFDATA}
        COMMAND ${LLVM_PROFDATA} merge
                -sparse
                -o ${MERGED_PROFDATA}
                ${COVERAGE_PROFRAW_DIR}/*.profraw
        DEPENDS ${test_profraw_files}
    )

    # The llvm-cov tool expects additional binaries to use the -object flag
    # (using a wild-card of the binaries doesn't work) so stitch together
    # a string of all the binaries separated by ' -object '. Note that the
    # generator expression ($<...>) gets evaluated during the command.
    foreach(test_target ${ARG_TEST_EXES})
        if(llvm_cov_bins)
            set(llvm_cov_bins ${llvm_cov_bins} -object )
        endif()
        set(llvm_cov_bins ${llvm_cov_bins} $<TARGET_FILE:${test_target}>)
    endforeach()

    # Use a format-specific directory for the coverage report.
    set(COVERAGE_OUTPUT_DIR ${COVERAGE_DIR}/${ARG_FORMAT})
    if(${ARG_FORMAT} STREQUAL html)
        set(COVERAGE_OUTPUT ${COVERAGE_OUTPUT_DIR}/index.html)
    elseif(${ARG_FORMAT} STREQUAL text)
        set(COVERAGE_OUTPUT ${COVERAGE_OUTPUT_DIR}/index.txt)
    else()
        message(FATAL_ERROR "Unsupported format '{ARG_FORMAT}'")
    endif()

    # Generate HTML coverage report from the merged profraw file using llvm-cov.
    add_custom_command(
        OUTPUT ${COVERAGE_OUTPUT}
        # Wipe the coverage report files to force llvm-cov to re-generate them.
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${COVERAGE_OUTPUT_DIR}
        COMMAND ${LLVM_COV} show ${llvm_cov_bins}
                -instr-profile=${MERGED_PROFDATA}
                -format ${ARG_FORMAT}
                -output-dir ${COVERAGE_OUTPUT_DIR}
        DEPENDS ${MERGED_PROFDATA}
    )

    # Add a target that the user can invoke to generate a coverage report.
    add_custom_target(coverage_${ARG_FORMAT} DEPENDS ${COVERAGE_OUTPUT})

    # Allow the caller to find their coverage report.
    set(COVERAGE_OUTPUT_${ARG_FORMAT} ${COVERAGE_OUTPUT} PARENT_SCOPE)

    message(STATUS "Enabling coverage; build coverage_${ARG_FORMAT} to generate the report.")
endfunction()
