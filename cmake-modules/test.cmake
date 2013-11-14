# This script expects the following variables to be set beforehand:
#
# CTEST_CMAKE_GENERATOR
# CTEST_SOURCE_DIRECTORY
# CTEST_BINARY_DIRECTORY
# CTEST_SITE
# MARSYAS_CONFIGURATION

include(${CTEST_SCRIPT_DIRECTORY}/utilities.cmake)

if(NOT MARSYAS_CONFIGURATION)
  message(FATAL_ERROR "Missing MARSYAS_CONFIGURATION option.")
endif()

set(CONFIGURATION_FILE "${CTEST_SCRIPT_DIRECTORY}/test_config_${MARSYAS_CONFIGURATION}.cmake")

if(NOT EXISTS ${CONFIGURATION_FILE})
  message(FATAL_ERROR "File for configuration '${MARSYAS_CONFIGURATION}' does not exist: ${CONFIGURATION_FILE}")
endif()

include(${CONFIGURATION_FILE})

#set(WITH_MEMCHECK TRUE)
#set(WITH_COVERAGE TRUE)

#######################################################################

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
