## global setup
cmake_minimum_required (VERSION 2.6)
project (Marsyas)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake-modules")

## basic CMAKE vars
mark_as_advanced (EXECUTABLE_OUTPUT_PATH)
mark_as_advanced (LIBRARY_OUTPUT_PATH)
if (APPLE)
	mark_as_advanced (CMAKE_OSX_ARCHITECTURES)
	mark_as_advanced (CMAKE_OSX_SYSROOT)
endif (APPLE)

set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


## find OS
if(UNIX)
	if(APPLE)
		set(MARSYAS_MACOSX 1)
	elseif(CYGWIN)
		set(MARSYAS_CYGWIN 1)
	elseif(UNIX)
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

## CONFIG.H
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})

## setup includes
include (marsyas-includes)

## setup oslibs
include (marsyas-oslibs)


## next level
add_subdirectory(marsyas)
if (BUILD_APPS)
  add_subdirectory(apps)
endif (BUILD_APPS)

if (SWIG_FOUND) 
	add_subdirectory(swig)
endif (SWIG_FOUND)

if (QT4_FOUND)
	include (${QT_USE_FILE})
        add_subdirectory(marsyasqt)
        add_subdirectory(Qt4Apps)
endif (QT4_FOUND)

if (VAMP_FOUND)
	add_subdirectory(mvamp)
endif (VAMP_FOUND)


## tests, for verification
enable_testing()
add_subdirectory (tests)


