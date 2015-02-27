cmake_minimum_required(VERSION 2.8.9)
# Note: Policies have to be repeated after every minimum cmake version declaration
if(POLICY CMP0020)
  # Accept new policy related to automatically linking Qt apps to qtmain.lib on Windows
  cmake_policy(SET CMP0020 NEW)
endif()

# Accept policy to ignore COMPILE_DEFINITIONS_<CONFIG> properties of targets and directories
# (Instead, use generator expressions for configuration-dependent compile definitions)
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

# Accept policy: use rpath on Mac OS X by default
if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

# Mac OS X: rpath handling
set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "@loader_path/../lib")



option(MARSYAS_DOCUMENTATION_ONLY "Only build documentation, not program sources" OFF)

# If only building documentation, tell CMake
# not to check for any language compilers.
if(MARSYAS_DOCUMENTATION_ONLY)
  set(MARSYAS_LANGUAGES NONE)
endif()



project(marsyas ${MARSYAS_LANGUAGES})

set(MARSYAS_VERSION_MAJOR "0")
set(MARSYAS_VERSION_MINOR "5")
set(MARSYAS_VERSION_PATCH "0")
set(MARSYAS_VERSION
  ${MARSYAS_VERSION_MAJOR}.${MARSYAS_VERSION_MINOR}.${MARSYAS_VERSION_PATCH})

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
