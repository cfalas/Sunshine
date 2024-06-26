cmake_minimum_required(VERSION 3.19)

# find doxygen and graphviz
find_package(Doxygen
        REQUIRED dot)

if(POLICY CMP0094)  # https://cmake.org/cmake/help/latest/policy/CMP0094.html
    cmake_policy(SET CMP0094 NEW)  # FindPython should return the first matching Python
endif()

# needed on GitHub Actions CI: actions/setup-python does not touch registry/frameworks on Windows/macOS
# this mirrors PythonInterp behavior which did not consult registry/frameworks first
if(NOT DEFINED Python_FIND_REGISTRY)
    set(Python_FIND_REGISTRY "LAST")  # cmake-lint: disable=C0103
endif()
if(NOT DEFINED Python_FIND_FRAMEWORK)
    set(Python_FIND_FRAMEWORK "LAST")  # cmake-lint: disable=C0103
endif()

# find python
if(NOT DEFINED Python_EXECUTABLE)
    find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
else()
    set(Python3_EXECUTABLE "${Python_EXECUTABLE}")  # cmake-lint: disable=C0103
    message(STATUS "Using Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
endif()

# create venv in build dir
set(VENV_DIR "${CMAKE_BINARY_DIR}/venv")

# create venv
execute_process(
    COMMAND ${Python3_EXECUTABLE} -m venv ${VENV_DIR}
    COMMAND_ERROR_IS_FATAL LAST
    RESULT_VARIABLE VENV_RESULT
)

# set Python3_EXECUTABLE to the python interpreter in the venv
if(WIN32)
    set(Python3_EXECUTABLE "${VENV_DIR}/Scripts/python.exe")  # cmake-lint: disable=C0103
    set(Python3_ROOT_DIR "${VENV_DIR}/Scripts")  # cmake-lint: disable=C0103
else()
    set(Python3_EXECUTABLE "${VENV_DIR}/bin/python")  # cmake-lint: disable=C0103
    set(Python3_ROOT_DIR "${VENV_DIR}/bin")  # cmake-lint: disable=C0103
endif()

# print path
message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")

# call a simple python command
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "print('   Testing Python3_EXECUTABLE')"
    COMMAND_ERROR_IS_FATAL ANY
    RESULT_VARIABLE PYTHON_RESULT
)

# fail if the python command failed
if(NOT PYTHON_RESULT EQUAL 0)
    message(FATAL_ERROR "Python3_EXECUTABLE failed")
endif()

# install doc requirements
execute_process(
    COMMAND ${Python3_EXECUTABLE} -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    COMMAND_ERROR_IS_FATAL ANY
    RESULT_VARIABLE PIP_RESULT
)

# rst check
set(RST_PATTERNS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/README.rst
)

# check rst files
foreach(pattern ${RST_PATTERNS})
    message(STATUS "Checking RST files: ${pattern}")
    execute_process(
        COMMAND "${Python3_ROOT_DIR}/rstcheck" -r "${pattern}"
        COMMAND_ERROR_IS_FATAL ANY
        RESULT_VARIABLE RST_RESULT
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endforeach()

# commands
list(APPEND SPHINX_BUILD_HTML_COMMAND
        "${Python3_ROOT_DIR}/sphinx-build"
        "-M"
        "html"
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_BINARY_DIR}/build/html"
        "-W"
        "--keep-going"
)

list(APPEND SPHINX_BUILD_EPUB_COMMAND
        "${Python3_ROOT_DIR}/sphinx-build"
        "-M"
        "epub"
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_BINARY_DIR}/build/epub"
        "-W"
        "--keep-going"
)

# build docs
add_custom_target(docs ALL
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Building documentation"
        COMMAND ${SPHINX_BUILD_HTML_COMMAND}
        COMMAND ${SPHINX_BUILD_EPUB_COMMAND}
        VERBATIM
)
