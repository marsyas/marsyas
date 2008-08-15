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
##### temp -- will be deleted when we get rid of autoconf
add_definitions(-DCMAKE_CONFIG_H)


## global defines
include (marsyas-global)


## next level
add_subdirectory(marsyas)
add_subdirectory(apps)

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

# directory possibly to be changed
add_subdirectory (test/unit_test)
include (marsyas-tests)

