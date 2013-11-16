# This module helps writing CTest scripts for Marsyas.
#
# Instructions for use in a CTest script:
#
# 1. Set global parameters by setting CTest variables:
#    CTEST_SITE = The name of the machine. Will appear on CDash.
#    CTEST_SOURCE_DIRECTORY = Marsyas source directory.
#    CTEST_BINARY_DIRECTORY = Desired build directory.
#    CTEST_CMAKE_GENERATOR = CMake generator (e.g. "Unix Makefiles", "Visual Studio 12 Win64")
#    CTEST_CONFIGURATION_TYPE = CMake configuration (e.g. "Debug")
#
# 2. Include this module
#
# 3. Use the marsyas_test_experimental function for each test
#    using a different set of CMake options:
#
#    marsyas_test_experimental(name branch options)
#    * name = Test name. Will appear on CDash
#    * branch = Git branch to test (does not work yet, always uses current branch)
#    * options = A list of CMake options, e.g. "-DMARSYAS_AUDIOIO=OFF"


# Make directories absolute, else it can confuse some commands:
get_filename_component(CTEST_SOURCE_DIRECTORY "${CTEST_SOURCE_DIRECTORY}" ABSOLUTE)
get_filename_component(CTEST_BINARY_DIRECTORY "${CTEST_BINARY_DIRECTORY}" ABSOLUTE)

find_program(CTEST_GIT_COMMAND NAMES git)
set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")

function(marsyas_test_experimental name branch options)

  if(DEFINED CTEST_CONFIGURATION_TYPE)
    list(APPEND options "-DCMAKE_BUILD_TYPE=${CTEST_CONFIGURATION_TYPE}")
  endif()

  list(APPEND options "-DMARSYAS_TESTS=ON")

  set(CTEST_BUILD_NAME "${branch}-${name}")

  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

  ctest_start(Experimental)
  ctest_update()
  ctest_configure(OPTIONS "${options}")
  ctest_build()
  ctest_test()
  ctest_submit()

endfunction()
