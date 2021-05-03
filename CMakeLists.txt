cmake_minimum_required(VERSION 3.16)

# Project information.
project(
    Template-C
    VERSION 0.0.1
    DESCRIPTION "A cookie-cutter project for writing C code"
)
set(PROJECT_AUTHORS "Matt Law")
set(PROJECT_RELEASE ${CMAKE_PROJECT_VERSION}-alpha)
set(PROJECT_COPYRIGHT "2021, ${PROJECT_AUTHORS}")

# CMake user-facing options.
option(BUILD_DOCS "Build documentation; set to OFF to disable." ON)
option(CLANG_TIDY "Run clang-tidy during the build process." OFF)
option(COLOR "Set to OFF to turn off colored output during build." ON)
option(COVERAGE "Generate test coverage report; set to ON to enable." OFF)
option(UWP "Universal Windows Platform; set to ON to enable." OFF)

# Create a <build>/compile_commands.json file that tools can read
# to get the compilation flags for building the project.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use ISO version 17 of the C & C++ standards.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use a release build as the default.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add a profile build type for profiling the code.
if(DEFINED MSVC)
    set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELWITHDEBINFO} /Oy-")
    set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Oy-")
else()
    set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELWITHDEBINFO} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fno-omit-frame-pointer")
endif()

# Add an address sanitizer build type.
set(CMAKE_C_FLAGS_ASAN "${CMAKE_C_FLAGS_PROFILE} -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS_PROFILE} -fsanitize=address -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS_ASAN "${CMAKE_EXE_LINKER_FLAGS_ASAN} -fsanitize=address -fsanitize=undefined")
set(CMAKE_SHARED_LINKER_FLAGS_ASAN "${CMAKE_SHARED_LINKER_FLAGS_ASAN} -fsanitize=address -fsanitize=undefined")

# Add a thread sanitizer build type.
set(CMAKE_C_FLAGS_TSAN "${CMAKE_C_FLAGS_PROFILE} -fsanitize=thread -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_TSAN "${CMAKE_CXX_FLAGS_PROFILE} -fsanitize=thread -fsanitize=undefined")

# Hide the options from the CMake GUI.
mark_as_advanced(
    CMAKE_C_FLAGS_PROFILE
    CMAKE_CXX_FLAGS_PROFILE
    CMAKE_C_FLAGS_ASAN
    CMAKE_CXX_FLAGS_ASAN
    CMAKE_EXE_LINKER_FLAGS_ASAN
    CMAKE_SHARED_LINKER_FLAGS_ASAN
    CMAKE_C_FLAGS_TSAN
    CMAKE_CXX_FLAGS_TSAN
)

# Enable most sensible warnings for all targets.
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wdouble-promotion
    -Wnull-dereference
    -Wformat=2
    -Werror
    -Wfatal-errors
)

# Create a define for UWP so that some code, like env_get can be disabled
# when targeting the Universal Windows Platform.
if(UWP)
    add_compile_definitions(UNIVERSAL_WINDOWS_PLATFORM)
endif()

# Add Windows specific compilation flags.
if(WIN32)
    # Microsoft renamed POSIX functions and deprecated the original names,
    # although the functions are still supported. Disable the deprecation
    # warnings about this so we can use the POSIX names on both platforms.
    add_compile_definitions(_CRT_NONSTDC_NO_WARNINGS)
endif()

# Add color output to the build unless told otherwise.
# Doesn't work for VSCode's CMake/Build output terminal :(
if(COLOR)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        add_compile_options(-fdiagnostics-color=always)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

# Enable clang-tidy integration with the build system.
if(CLANG_TIDY)
    find_program(CLANG_TIDY_EXE "clang-tidy" REQUIRED)
    message(STATUS "Enabling clang-tidy: ${CLANG_TIDY_EXE}")
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE})
endif()

include(GNUInstallDirs)

# Use a default install directory in the build tree to avoid accidentally
# installing to the system's path during development. This also lets users
# inspect the files they wish to install before installing them.
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)

add_subdirectory(src)

if(BUILD_DOCS)
    message(STATUS "Enabling documentation; set -DBUILD_DOCS=OFF to disable.")
    add_subdirectory(doc)
endif()

include(CTest)
if(BUILD_TESTING) # defined by CTest
    message(STATUS "Enabling tests; set -DBUILD_TESTING=OFF to disable.")
    add_subdirectory(test)
else()
    if(COVERAGE)
        message(FATAL_ERROR "COVERAGE enabled but BUILD_TESTING disabled.")
    endif()
endif()
