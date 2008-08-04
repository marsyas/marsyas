## global setup
cmake_minimum_required(VERSION 2.6)
project(Marsyas)
mark_as_advanced (EXECUTABLE_OUTPUT_PATH)
mark_as_advanced (LIBRARY_OUTPUT_PATH)

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
	if(MARSYAS_AUDIOIO)
	   set(__WINDOWS_DS__)
        endif 
endif(UNIX)

include (TestBigEndian)
test_big_endian(MARSYAS_BIGENDIAN)


## user options
option(MARSYAS_AUDIOIO "Build the audio I/O interface" ON)
option(MARSYAS_MIDIIO "Build the MIDI I/O interface" ON)
if (MARSYAS_LINUX)
	option(OSS "[LINUX ONLY] Use OSS instead of ALSA" OFF)
endif (MARSYAS_LINUX)


# FIXME: waiting to confirm
option(MARSYAS_ASSERT "Build with assertions" OFF)
mark_as_advanced (MARSYAS_ASSERT)
option(MARSYAS_PROFILING "Build with profiling" OFF)
mark_as_advanced (MARSYAS_PROFILING)
option(MARSYAS_DEBUG "Build with debugging info (large performance penalty)" OFF)

option(MARSYAS_LOG_WARNINGS "Build with warnings" OFF)
mark_as_advanced (MARSYAS_LOG_WARNINGS)
option(MARSYAS_LOG_DEBUGS "Log debugs" OFF)
mark_as_advanced (MARSYAS_LOG_DEBUGS)
option(MARSYAS_LOG_DIAGNOSTICS "Log debugs" OFF)
mark_as_advanced (MARSYAS_LOG_DIAGNOSTICS)
option(MARSYAS_LOG2FILE "Log messages to a file" OFF)
mark_as_advanced (MARSYAS_LOG2FILE)
option(MARSYAS_LOG2STDOUT "Log messages to a standard out" OFF)
mark_as_advanced (MARSYAS_LOG2STDOUT)
option(MARSYAS_LOG2GUI "Log messages to GUI" OFF)
mark_as_advanced (MARSYAS_LOG2GUI)

# not used?!?!
#option(DISTRIBUTED "[EXPERIMENTAL] compile code for distributed
#audio processing" OFF)


#### will finish these later -- they all require special checks
#### and whatnot.
option(WITH_MAD "Link to libmad (mp3 audio decoding)" OFF)
#option(WITH_VORBIS "Link to libvorbis (ogg audio decoding)" OFF)
#option(WITH_READLINE "Readline functionality for Marsyas Scripting
#Language (MSL)" OFF)
#option(WITH_MATLAB "Activate MATLAB enging interface" OFF)
#option(WITH_SWIG "Enable SWIG generated bindings" OFF)
option(WITH_QT "Build the QT library and applications" OFF)
#option(WITH_QGL "Enable qglviewer (Qt-based OpenGL viewer)" OFF)


## DETECT PACKAGES
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

if (WITH_QT)
	include(FindQt4)
endif (WITH_QT)

if (WITH_MAD)
	find_package(mad REQUIRED)
	set (MARSYAS_MAD 1)
endif (WITH_MAD)
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

if(QT4_FOUND)
        add_subdirectory(marsyasqt)
        add_subdirectory(Qt4Apps)
endif(QT4_FOUND)

