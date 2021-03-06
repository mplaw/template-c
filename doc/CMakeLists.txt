# Generate documentation.
#
# Creating the documentation is a two-stage process:
#
#   1. Doxygen is used to generate XML files from the C source code.
#      https://cmake.org/cmake/help/latest/module/FindDoxygen.html
#      https://www.doxygen.nl/manual/config.html
#
#   2. Sphinx and Breathe read the XML files from step 1 and create
#      HTML (or other) user-facing documentation.
#      https://www.sphinx-doc.org/en/master/
#      https://breathe.readthedocs.io/en/latest/index.html
#
# Doxygen is a powerful documentation generation tool in its own right but the
# HTML output it produces isn't very nice, hence we also use Sphinx and Breathe.

# User-facing options.
set(DOCS_OUT_TYPES "html" CACHE STRING
    "A list of output types for documentation, the most common ones being html and man.
    choices: html;man;dirhtml;singlehtml;htmlhelp;qthelp;applehelp;devhelp;epub;latex;text;texinfo;changes;gettext;json;pickle;linkcheck;pseudoxml;xml
    See https://www.sphinx-doc.org/en/master/usage/builders/index.html for all possible choices.")

# Use CMake's built-in Doxygen package.
find_package(Doxygen REQUIRED)

# Create only XML output for breathe.
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)

# Put Doxygen's output in its own directory so Sphinx doesn't overwrite it.
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")

# Find all the public headers and add a doxygen target that makes documentation
# from them. Note that finding the public headers happens at configure time, so
# adding or removing new headers won't cause the documentation to be re-built
# unless the build is configured again.
file(GLOB_RECURSE PROJECT_PUBLIC_HEADERS ${CMAKE_SOURCE_DIR}/src/include/*)
list(JOIN PROJECT_PUBLIC_HEADERS " " DOXYGEN_INPUT)

set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

add_custom_command(
    OUTPUT ${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    DEPENDS ${PROJECT_PUBLIC_HEADERS}
    COMMENT "Generating XML files with doxygen."
    VERBATIM
)

# Setup a Python virtual environment (venv) for Sphinx and breathe.
#
# The venv is installed as part of the build so we can't look-up the paths
# to it at configure time and instead have to predict the locations of the
# venv binaries. The name of the pip binary is particularly unpredictable
# as it depends on the version of Python (some versions of Python don't
# also create the "pip" symlink), so refer to access pip we activate the
# venv and then use "python -m pip".
if(WIN32)
    set(EXE_SUFFIX ".exe")
    set(VENV_BIN ${CMAKE_CURRENT_BINARY_DIR}/venv/Scripts)
    set(VENV_ACTIVATE ${CMAKE_CURRENT_BINARY_DIR}/venv/Scripts/activate.bat) # assumes cmd
else()
    set(EXE_SUFFIX "")
    set(VENV_BIN ${CMAKE_CURRENT_BINARY_DIR}/venv/bin)
    set(VENV_ACTIVATE ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/activate.sh)
endif()
set(VENV_PYTHON ${VENV_BIN}/python${EXE_SUFFIX})
set(SPHINX_EXECUTABLE ${VENV_BIN}/sphinx-build)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
add_custom_command(
    OUTPUT ${VENV_PYTHON}
    COMMAND Python3::Interpreter -m venv venv
    COMMENT "Creating a Python venv for Sphinx and breathe"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Install the Python packages required for the documentation to the venv.
add_custom_command(
    OUTPUT requirements.stamp
    COMMAND ${VENV_ACTIVATE}
    COMMAND python -m pip install --upgrade -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    COMMAND ${CMAKE_COMMAND} -E touch requirements.stamp
    COMMENT "Installing Python packages to venv"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    DEPENDS ${VENV_PYTHON}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_target(doc_venv DEPENDS ${VENV_PYTHON} requirements.stamp)

# Populate conf.py with project information (e.g. name, version, etc...)
set(SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
configure_file(conf.py conf.py @ONLY)

# Find all the reStructuredText files that sphinx uses.
file(GLOB_RECURSE PROJECT_RST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.rst)
list(APPEND PROJECT_RST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                              ${CMAKE_SOURCE_DIR}/readme.rst)

# Add a target for generating a single type of documentation.
#
#   type
#       The sphinx builder to use, e.g. html or man. See
#       https://www.sphinx-doc.org/en/master/usage/builders/index.html
#       for a full list of possible builders.
#
#   output
#       A file that gets created as part of the documentation generation.
#       This is used to avoid rebuilding unless the file's modification time
#       is newer than the documentation's modification time.
#
function(docs_add_command type output)
    set(SPHINX_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(SPHINX_DST_DIR ${CMAKE_CURRENT_BINARY_DIR}/${type})
    set(SPHINX_OUTPUT ${SPHINX_DST_DIR}/${output})

    if (${type} STREQUAL html)
        # Sphinx copies these files to the build directory.
        set(HTML_OUTPUTS ${SPHINX_DST_DIR}/_static/favicon.svg
                         ${SPHINX_DST_DIR}/_static/logo.svg)
    endif()

    add_custom_command(
        OUTPUT ${SPHINX_OUTPUT}
               ${HTML_OUTPUTS}
        COMMAND ${VENV_ACTIVATE} && ${SPHINX_EXECUTABLE}
                -a
                -j auto
                -b ${type}
                -c ${CMAKE_CURRENT_BINARY_DIR}
                -Dbreathe_projects.${PROJECT_NAME}=${DOXYGEN_OUTPUT_DIRECTORY}/xml
                ${SPHINX_SRC_DIR}
                ${SPHINX_DST_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/conf.py
        DEPENDS ${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml
                requirements.stamp
                _static/style.css
                _static/logo.svg
                _static/favicon.svg
                _templates/layout.html
                ${PROJECT_RST_FILES}
        COMMENT "Generating ${type} documentation"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_target(
        docs_${type}
        DEPENDS ${SPHINX_OUTPUT}
    )
    install(
        DIRECTORY ${SPHINX_DST_DIR}
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )
endfunction()

# Return a file that gets generated as part of building the docs for `type`.
# This is useful so the build system can figure out when it doesn't need to
# rebuild the docs, i.e. because the target file is newer than the sources.
function(docs_get_out_file type)
    if(${type} STREQUAL html OR ${type} STREQUAL htmlhelp OR
       ${type} STREQUAL dirhtml OR ${type} STREQUAL qthelp OR
       ${type} STREQUAL singlehtml OR ${type} STREQUAL devhelp)
        set(OUT_FILE index.html PARENT_SCOPE)
    elseif(${type} STREQUAL man)
        set(OUT_FILE template-c.1 PARENT_SCOPE)
    elseif(${type} STREQUAL json)
        set(OUT_FILE index.fjson PARENT_SCOPE)
    elseif(${type} STREQUAL pickle)
        set(OUT_FILE index.fpickle PARENT_SCOPE)
    elseif(${type} STREQUAL applehelp)
        set(OUT_FILE ${PROJECT_NAME}.help/Info.plist PARENT_SCOPE)
    elseif(${type} STREQUAL latex)
        set(OUT_FILE ${PROJECT_NAME}.tex PARENT_SCOPE)
    elseif(${type} STREQUAL texinfo)
        set(OUT_FILE ${PROJECT_NAME}.texi PARENT_SCOPE)
    elseif(${type} STREQUAL text)
        set(OUT_FILE index.txt PARENT_SCOPE)
    elseif(${type} STREQUAL gettext)
        set(OUT_FILE index.pot PARENT_SCOPE)
    elseif(${type} STREQUAL epub)
        set(OUT_FILE index.xhtml PARENT_SCOPE)
    elseif(${type} STREQUAL linkcheck)
        set(OUT_FILE output.txt PARENT_SCOPE)
    elseif(${type} STREQUAL changes)
        set(OUT_FILE .doctrees/index.doctree PARENT_SCOPE)
    else()
        # works for html, pseudoxml, xml
        set(OUT_FILE "index.${type}" PARENT_SCOPE)
    endif()
endfunction()

# Add a docs_<name> target for each argument this function is passed.
function(docs_add_all)
    set(DOCS_ALL_TARGETS "")
    foreach(type ${ARGN})
        docs_get_out_file(${type})
        docs_add_command(${type} ${OUT_FILE})
        list(APPEND DOCS_ALL_TARGETS docs_${type})
    endforeach()
    set(DOCS_ALL_TARGETS ${DOCS_ALL_TARGETS} PARENT_SCOPE)
endfunction()

# Add documentation for each output type we're interested in.
docs_add_all(${DOCS_OUT_TYPES})

# Add a single target to generate all the documentation types.
add_custom_target(docs ALL DEPENDS ${DOCS_ALL_TARGETS})

# Build the docs like Read The Docs does.
#
# Read The Docs' build process is documented here:
# https://docs.readthedocs.io/en/stable/builds.html
# Note that the build command has been slightly modified to not pollute the source tree.
#
# This is essentially a test to check changes to the documentation build system
# haven't broke anything. As such only rebuild this when the documentation build
# system changes rather than when the documentation itself changes.
set(RTD_DOCS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc/html_rtd)

add_custom_command(
    OUTPUT ${RTD_DOCS_OUTPUT_DIR}/index.html
    COMMAND ${VENV_ACTIVATE} &&
            ${CMAKE_COMMAND} -E env READTHEDOCS=True READTHEDOCS_PROJECT=${CMAKE_PROJECT_NAME}
                                    READTHEDOCS_VERSION=latest READTHEDOCS_LANGUAGE=en
            python -m sphinx -T -E -b html -D language=en . ${RTD_DOCS_OUTPUT_DIR} &&
            ${CMAKE_COMMAND} -E echo "Manually inspect ${RTD_DOCS_OUTPUT_DIR}"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/conf.py
    DEPENDS requirements.stamp
    COMMENT "Testing documentation can be built as RTD does"
    # RTD's cwd is the location of the conf.py file.
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc
)

add_custom_target(docs_rtd DEPENDS ${RTD_DOCS_OUTPUT_DIR}/index.html)
