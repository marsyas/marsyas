# This script should be run with the following variable set:
#
# MARSYAS_CONFIGURATION - The name of the current build configuration.
#
# The above variable will make part of the build name reported on CDash.
# Moreover, it will make this script include another "configuration" script
# with filename "test_config_${MARSYAS_CONFIGURATION}.cmake".
#
# The configuration script should set the following variables:
#
# CTEST_SITE - The name of the build site (machine)
# CTEST_SOURCE_DIRECTORY - Marsyas source dir
# CTEST_BINARY_DIRECTORY - Desired build dir
# CTEST_CMAKE_GENERATOR - CMake generator to use (e.g. "Unix Makefiles", "Visual Studio 12 Win64")
# CTEST_BUILD_CONFIGURATION (optional) - CMake configuration (e.g. "Debug")
# CTEST_BUILD_OPTIONS (optional) - CMake options (e.g. "-DMARSYAS_AUDIOIO=OFF")
# MARSYAS_BRANCH (optional) - CMake branch to build (not yet working, always builds current branch)

#include(${CTEST_SCRIPT_DIRECTORY}/utilities.cmake)

if(NOT MARSYAS_CONFIGURATION)
  message(FATAL_ERROR "Missing MARSYAS_CONFIGURATION option.")
endif()

set(CONFIGURATION_FILE "${CTEST_SCRIPT_DIRECTORY}/test_config_${MARSYAS_CONFIGURATION}.cmake")

if(NOT EXISTS ${CONFIGURATION_FILE})
  message(FATAL_ERROR "File for configuration '${MARSYAS_CONFIGURATION}' does not exist: ${CONFIGURATION_FILE}")
endif()

include(${CONFIGURATION_FILE})

if(NOT DEFINED MARSYAS_BRANCH)
	set(MARSYAS_BRANCH "master")
	message(STATUS "Maryas branch defaults to '${MARSYAS_BRANCH}'")
endif()

#set(WITH_MEMCHECK TRUE)
#set(WITH_COVERAGE TRUE)

#######################################################################

set(CTEST_BUILD_NAME "${MARSYAS_BRANCH}-${MARSYAS_CONFIGURATION}")

ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

find_program(CTEST_GIT_COMMAND NAMES git)

if(WITH_COVERAGE)
  find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
endif()

if(WITH_MEMCHECK)
  find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)
  set(CTEST_MEMORYCHECK_SUPPRESSIONS_FILE ${CTEST_SOURCE_DIRECTORY}/tests/valgrind.supp)
endif()

#if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}")
  #set(CTEST_CHECKOUT_COMMAND "${CTEST_GIT_COMMAND} clone https://github.com/marsyas/marsyas.git ${CTEST_SOURCE_DIRECTORY}")
#endif()

# The following command must be just the git executable, without arguments:
set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")
#set(CTEST_UPDATE_OPTIONS "origin ${MARSYAS_BRANCH}")

list(APPEND CTEST_BUILD_OPTIONS
  -DCMAKE_BUILD_TYPE=${CTEST_BUILD_CONFIGURATION}
  -DMARSYAS_TESTS=ON
)

ctest_start(Experimental)
ctest_update()
ctest_configure(OPTIONS "${CTEST_BUILD_OPTIONS}")
ctest_build()
ctest_test()
if (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
  ctest_coverage()
endif (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
if (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
  ctest_memcheck()
endif (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
ctest_submit()
