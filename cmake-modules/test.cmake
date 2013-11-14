# This script expects the following variables to be set beforehand:
#
# CTEST_CMAKE_GENERATOR
# CTEST_SOURCE_DIRECTORY
# CTEST_BINARY_DIRECTORY
# CTEST_SITE
# MARSYAS_CONFIGURATION

include(${CTEST_SCRIPT_DIRECTORY}/utilities.cmake)

if(MARSYAS_CONFIGURATION STREQUAL "Default")
  include(${CTEST_SCRIPT_DIRECTORY}/test_config_default.cmake)
elseif(MARSYAS_CONFIGURATION STREQUAL "Complete")
  include(${CTEST_SCRIPT_DIRECTORY}/test_config_complete.cmake)
elseif(MARSYAS_CONFIGURATION STREQUAL "Minimal")
  include(${CTEST_SCRIPT_DIRECTORY}/test_config_minimal.cmake)
else()
  message(FATAL_ERROR "Unknown Marsyas configuration: '${MARSYAS_CONFIGURATION}'")
endif()

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

list_to_space_separated_string(CTEST_BUILD_OPTIONS_STRING "${CTEST_BUILD_OPTIONS}")

set(CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CTEST_BUILD_CONFIGURATION}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -DMARSYAS_TESTS=ON ${CTEST_BUILD_OPTIONS_STRING}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} \"-G${CTEST_CMAKE_GENERATOR}\"")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\"")

ctest_start(Nightly)
ctest_update()
ctest_configure()
ctest_build()
ctest_test()
if (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
  ctest_coverage()
endif (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
if (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
  ctest_memcheck()
endif (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
ctest_submit()
