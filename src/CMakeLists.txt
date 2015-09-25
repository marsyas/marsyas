
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  message(FATAL_ERROR "CMake initialized in the wrong directory. Please initialize CMake with the top-level Marsyas directory as source directory.")
endif()

## basic CMAKE vars
if (APPLE)
  mark_as_advanced (CMAKE_OSX_ARCHITECTURES)
  mark_as_advanced (CMAKE_OSX_SYSROOT)
endif (APPLE)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

## find OS
if(UNIX)
  if(APPLE)
    set(MARSYAS_MACOSX 1)
  elseif(CYGWIN)
    set(MARSYAS_CYGWIN 1)
  else(APPLE)
    set(MARSYAS_LINUX 1)
  endif(APPLE)
elseif(MINGW)
  set(MARSYAS_MINGW 1)
elseif(MSVC)
  set(MARSYAS_WIN32 1)
endif(UNIX)

# for testing
#MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: LINUX " ${MARSYAS_LINUX})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: OSX " ${MARSYAS_MACOSX})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: WIN32 " ${MARSYAS_WIN32})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: MINGW " ${MARSYAS_MINGW})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: CYGWIN " ${MARSYAS_CYGWIN})

include (TestBigEndian)
test_big_endian(MARSYAS_BIGENDIAN)

## user options
include (marsyas-options)

## detect packages
include (marsyas-detect)

## general setup

# Include current directory, so that include paths of form <marsyas/...>
# work just like when installed.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(WITH_CPP11)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
  endif()
endif()

## CONFIG.H
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})
## oops, don't install this!  -gp
#install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/marsyas)

if(MARSYAS_STATIC)
  set(MARSYAS_STATIC_FLAG 1)
else()
  set(MARSYAS_STATIC_FLAG 0)
endif()
configure_file(marsyas/export.h.in ${CMAKE_CURRENT_BINARY_DIR}/marsyas/export.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(otherlibs)

## setup includes
include (marsyas-includes)

## setup oslibs
include (marsyas-oslibs)

## next level

add_subdirectory(marsyas)

if (MARSYAS_BUILD_APPS)
  add_subdirectory(apps)
  if(WITH_QT)
    add_subdirectory(qt5apps)
  endif()
endif (MARSYAS_BUILD_APPS)

if (SWIG_FOUND)
  add_subdirectory(swig)
endif (SWIG_FOUND)

if(WITH_QT4)
  SET(QT_USE_QTNETWORK ON)
  SET(QT_USE_QTXML ON)
  if(WITH_OPENGL)
    SET(QT_USE_QTOPENGL TRUE)
  endif()
  include (${QT_USE_FILE})
  add_subdirectory(Qt4Apps)
endif()

if (VAMP_FOUND)
  add_subdirectory(mvamp)
endif (VAMP_FOUND)

if (MAXSDK_FOUND)
  add_subdirectory(marsyas_max-msp)
endif (MAXSDK_FOUND)

if (PD_FOUND)
  add_subdirectory(marsyas_pd)
endif (PD_FOUND)

## tests, for verification
if (MARSYAS_TESTS)
  add_subdirectory (tests)
endif (MARSYAS_TESTS)

## bundling
if(WIN32)
  include(InstallRequiredSystemLibraries)

  ## 'fixup_bundle' only needs one executable in a directory to fix up all the siblings.
  add_custom_target(bundle
    COMMAND ${CMAKE_COMMAND}
    -DMARSYAS_BUNDLE_EXECUTABLES=${CMAKE_INSTALL_PREFIX}/bin/bextract.exe
    -DMARSYAS_BUNDLE_DIRS=${CMAKE_LIBRARY_PATH}
    -P ${CMAKE_SOURCE_DIR}/cmake-modules/marsyas-bundle.cmake
  )
endif()

## packaging
include(marsyas-package)
