## global setup
cmake_minimum_required(VERSION 2.6)
project(Marsyas)

## find OS
if(UNIX)
  if(APPLE)
    set(MARSYAS_MACOSX 1)
  elseif(CYGWIN)
    set(MARSYAS_CYGWIN 1)
  else(UNIX)
    set(MARSYAS_LINUX 1)
  endif(APPLE)
elseif(MSVC)
  set(MARSYAS_WIN32 1)
endif(UNIX)

## user options
option(WITH_QT "Build the QT library and applications" ON)
option(MARSYAS_AUDIOIO "Build the audio I/O interface" ON)
option(MARSYAS_MIDIIO "Build the MIDI I/O interface" ON)


## DETECT PACKAGES
if(WITH_QT)
        include(FindQt4)
endif(WITH_QT)
#### not certain if this is necessary
#include(FindThreads)



## CONFIG.H
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})
##### temp -- will be deleted when we get rid of autoconf
add_definitions(-DCMAKE_CONFIG_H)


## next level
####### can't get this to work on OSX
#add_subdirectory(otherlibs)
add_subdirectory(marsyas)
add_subdirectory(apps)

if(QT4_FOUND)
        add_subdirectory(marsyasqt)
endif(QT4_FOUND)

