cmake_minimum_required(VERSION 2.8.2)

# If only building documentation, tell CMake
# not to check for any language compilers.
if(MARSYAS_DOCUMENTATION_ONLY)
  set(MARSYAS_LANGUAGES NONE)
endif()

project(marsyas ${MARSYAS_LANGUAGES})

set(MARSYAS_VERSION_MAJOR "0")
set(MARSYAS_VERSION_MINOR "5")
set(MARSYAS_VERSION_PATCH "0-alpha")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

include(${CMAKE_SOURCE_DIR}/cmake-modules/utilities.cmake)

if(NOT MARSYAS_DOCUMENTATION_ONLY)
  if (MARSYAS_TESTS)
    enable_testing()
    include(CTest)
  endif()

  add_subdirectory(src)
endif()

add_subdirectory(doc)
