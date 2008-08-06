## global setup
cmake_minimum_required(VERSION 2.6)
project(Marsyas)
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

include (TestBigEndian)
test_big_endian(MARSYAS_BIGENDIAN)


## user options
include (marsyas-options)


## DETECT PACKAGES

if (WITH_QT)
	include(FindQt4)
endif (WITH_QT)

if (WITH_SWIG) 
	include(FindSWIG)
endif (WITH_SWIG) 


if (WITH_MAD)
	find_package(mad REQUIRED)
	set (MARSYAS_MAD 1)
endif (WITH_MAD)



if (WITH_MATLAB) 
	include(FindMatlab)
endif (WITH_MATLAB) 

#### not certain if this is necessary
#include(FindThreads)



## CONFIG.H
# temp renaming -- will rename stuff in src/maryas/ once we go
# to cmake only.
#if (WITH_MAD)
#  set(MARSYAS_MAD 1)
#endif (WITH_MAD)
#if ()
#endif ()

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})
##### temp -- will be deleted when we get rid of autoconf
add_definitions(-DCMAKE_CONFIG_H)


set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

## next level
####### can't get this to work on OSX
#add_subdirectory(otherlibs)
add_subdirectory(marsyas)
add_subdirectory(apps)

if(SWIG_FOUND) 
	add_subdirectory(swig)
endif(SWIG_FOUND)

if(QT4_FOUND)
        add_subdirectory(marsyasqt)
        add_subdirectory(Qt4Apps)
endif(QT4_FOUND)



if (MATLAB_FOUND) 
	set (MARSYAS_MATLAB 1) 
endif (MATLAB_FOUND)
