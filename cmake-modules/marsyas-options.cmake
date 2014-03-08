#
# If you add a #define for .cpp code, make sure you modify
#    src/config.h.in
# as well!
#


 if(CMAKE_CONFIGURATION_TYPES)
   list(APPEND CMAKE_CONFIGURATION_TYPES Profile)
   list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Add the configurations that we need"
     FORCE)
 endif()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profile."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
# SET( CMAKE_BUILD_TYPE "" CACHE STRING
#     "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profile."
#     FORCE )

SET(CMAKE_CXX_FLAGS_PROFILE "-Wall -O3 -fPIC -finstrument-functions" CACHE STRING "Flags used for profiling")
SET(CMAKE_SHARED_LINKER_FLAGS_PROFILE "")
SET(CMAKE_EXE_LINKER_FLAGS_PROFILE "-lpthread")

MARK_AS_ADVANCED(CMAKE_CXX_FLAGS_PROFILE)
MARK_AS_ADVANCED(CMAKE_SHARED_LINKER_FLAGS_PROFILE)
MARK_AS_ADVANCED(CMAKE_EXE_LINKER_FLAGS_PROFILE)





IF(CMAKE_COMPILER_IS_GNUCXX)
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")
  ELSE ()
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
  ENDIF ()
#	SET(CMAKE_CXX_FLAGS "-Wall -D_GLIBCXX_DEBUG")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

### USER OPTIONS

# Detect whether compiler version supports C++11
set(min_gnu_version 4.7.0)
set(min_clang_version 3.2.0)
set(min_apple_clang_version 4.2.0)
set(min_msvc_version 18)
set(min_ms_vs_version 2013)

if( DEFINED CMAKE_CXX_COMPILER_VERSION )

  if( CMAKE_CXX_COMPILER_ID MATCHES GNU )
    set(MARSYAS_REQUIRED_COMPILER_VERSION ${min_gnu_version})
  elseif( CMAKE_CXX_COMPILER_ID MATCHES Clang )
    # Apple has own derived Clang, with different versioning
    if(APPLE)
      set(MARSYAS_REQUIRED_COMPILER_VERSION ${min_apple_clang_version})
    else()
      set(MARSYAS_REQUIRED_COMPILER_VERSION ${min_clang_version})
    endif()
  elseif( CMAKE_CXX_COMPILER_ID MATCHES MSVC )
    set(MARSYAS_REQUIRED_COMPILER_VERSION ${min_msvc_version})
  else()
    set(MARSYAS_REQUIRED_COMPILER_VERSION 0)
  endif()

  if( CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL ${MARSYAS_REQUIRED_COMPILER_VERSION} OR
      CMAKE_CXX_COMPILER_VERSION VERSION_GREATER ${MARSYAS_REQUIRED_COMPILER_VERSION} )
    set(DEFAULT_WITH_CPP11 ON)
  else()
    set(DEFAULT_WITH_CPP11 OFF)
  endif()

else()
  set(DEFAULT_WITH_CPP11 OFF)
endif()

if(NOT ${DEFAULT_WITH_CPP11})
  set(msg "The version of your compiler does not support C++11 features required by some parts of Marsyas.\n")
  set(msg "${msg}Minimum required compiler versions are:\n")
  set(msg "${msg}- GNU ${min_gnu_version}\n")
  set(msg "${msg}- Clang ${min_clang_version}\n")
  set(msg "${msg}- Apple Clang ${min_apple_clang_version}\n")
  set(msg "${msg}- Microsoft Visual C++ ${min_msvc_version} (Visual Studio ${min_ms_vs_version})\n")
  message(WARNING "${msg}")
endif()

option(WITH_CPP11 "Use C++11 features" ${DEFAULT_WITH_CPP11})

option(MARSYAS_AUDIOIO "Build the audio I/O interface" ${DEFAULT_WITH_CPP11})
option(MARSYAS_MIDIIO "Build the MIDI I/O interface" ON)
option(MARSYAS_INLINE "Inline functions" ON)

if(MARSYAS_LINUX)
  option(WITH_JACK "Support Jack audio backend." ON)
  option(WITH_ALSA "Support Alsa audio backend." ON)
  option(WITH_OSS "Support OSS audio backend." OFF)
endif()

if (MARSYAS_WIN32 OR MARSYAS_MINGW OR MARSYAS_CYGWIN)
  option(MARSYAS_ASIO "[WINDOWS ONLY] Use ASIO instead of DirectSound" OFF)
endif (MARSYAS_WIN32 OR MARSYAS_MINGW OR MARSYAS_CYGWIN)


## OPTIONAL CODE
option(MARSYAS_REGENERATE_SCRIPT_PARSER
"Regenerate code for Marsyas scripting language parsing whenever grammar rules are changed." OFF)

option(WITH_ANN "Build the ANN (Approximate Nearest Neighbours) library" OFF)

option(MARSYAS_BUILD_APPS "Build the applications" ON)
mark_as_advanced (MARSYAS_BUILD_APPS)

option(MARSYAS_TESTS "Build the tests" OFF)
if(MARSYAS_TESTS)
  find_path(TEST_MINI_GENRES_DIR
    mini-genres
    DOC "Directory for the mini-genres database"
    PATHS $ENV{MARSYAS_DATADIR})
endif (MARSYAS_TESTS)

if (MARSYAS_LINUX OR MARSYAS_MACOSX)
  option(MARSYAS_BUILD_DISTRIBUTED "[EXPERIMENTAL] compile code for
    distributed audio processing" OFF)
  mark_as_advanced(MARSYAS_BUILD_DISTRIBUTED)
  if (MARSYAS_BUILD_DISTRIBUTED)
    set(MARSYAS_DISTRIBUTED 1)
  endif (MARSYAS_BUILD_DISTRIBUTED)
endif (MARSYAS_LINUX OR MARSYAS_MACOSX)


## CODE MESSAGES
#mark_as_advanced (MARSYAS_PROFILING)
option(MARSYAS_TRACECONTROLS "Build with tracing control info" OFF)
mark_as_advanced (MARSYAS_TRACECONTROLS)
option(MARSYAS_FLOWCHECK "Check dataflow information" OFF)
mark_as_advanced (MARSYAS_FLOWCHECK)
option(MARSYAS_BOUNDCHECK "Check vector element access for boundary violations (large performance penalty)" OFF)
mark_as_advanced (MARSYAS_BOUNDCHECK)
option(MARSYAS_STATIC "Build as a static library" OFF)

## STL DEBUGGING
option(MARSYAS_ENABLE_CHECKED_STL "Enable bounds checking on STL code" OFF)
mark_as_advanced (MARSYAS_ENABLE_CHECKED_STL)

IF(MARSYAS_ENABLE_CHECKED_STL)
  SET(CMAKE_CXX_FLAGS "-Wall -D_GLIBCXX_DEBUG")
ENDIF(MARSYAS_ENABLE_CHECKED_STL)

## logging stuff
option(MARSYAS_LOG_WARNINGS "Log warnings" ON)
option(MARSYAS_LOG_DEBUGS "Log debugs" OFF)
option(MARSYAS_LOG_DIAGNOSTICS "Log diagnostics" OFF)
option(MARSYAS_LOG_ERRORS "Log errors" ON)
option(MARSYAS_LOG_MESSAGES "Log messages" ON)
option(MARSYAS_LOG2FILE "Log messages to a file" OFF)
option(MARSYAS_LOG2STDOUT "Log messages to a standard out" ON)
option(MARSYAS_LOG2STDERR "Log messages to a standard error" OFF)
option(MARSYAS_LOG2GUI "Log messages to GUI" OFF)
mark_as_advanced (MARSYAS_LOG2GUI)


## using other software
option(WITH_MAD "Link to libmad (mp3 audio decoding)" OFF)
option(WITH_LAME "Link to liblame (mp3 audio encoding)" OFF)
option(WITH_VORBIS "Link to libvorbis (ogg audio decoding)" OFF)
option(WITH_MATLAB "Activate MATLAB enging interface" OFF)
option(WITH_SWIG "Enable SWIG generated bindings" OFF)
option(WITH_QT4 "Build the Qt4 library and applications" OFF)
option(WITH_QT "Build the Qt5 applications" ON)
option(WITH_VAMP "Build plugins for Vamp" OFF)
option(WITH_MAX/MSP "Build externals for Max/MSP" OFF)
option(WITH_PD "Build externals for Pd" OFF)
option(WITH_PNG  "Build png writer" OFF)
option(WITH_OPENGL "Build programs that require OpenGL" OFF)
option(WITH_OPENGLUT "Build programs that require OpenGL Utility Toolkit" OFF)
option(WITH_GSTREAMER "Enable use of GStreamer as audio source" OFF)
#option(WITH_QGL "Enable qglviewer (Qt-based OpenGL viewer)" OFF)
#option(WITH_READLINE "Readline functionality for Marsyas Scripting
#Language (MSL)" OFF)

set(WITH_OSC ON)

## SWIG sub-options
if (WITH_SWIG)
  option(WITH_SWIG_PYTHON "Enable the SWIG bindings for Python" ON)
  option(WITH_SWIG_JAVA "Enable the SWIG bindings for Java
NOTWORKING" OFF)
  option(WITH_SWIG_LUA "Enable the SWIG bindings for Lua
NOTWORKING" OFF)
  option(WITH_SWIG_RUBY "Enable the SWIG bindings for Ruby
NOTWORKING" OFF)
endif (WITH_SWIG)


IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
# set(MARSYAS_ASSERTS ON CACHE BOOL "" FORCE)
set(MARSYAS_FLOWCHECK ON CACHE BOOL "" FORCE)
set(MARSYAS_TRACECONTROLS ON CACHE BOOL "" FORCE)
set(MARSYAS_INLINE OFF CACHE BOOL "" FORCE)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Check option compatibility

if(MARSYAS_AUDIOIO AND NOT WITH_CPP11)
  message(FATAL_ERROR "MARSYAS_AUDIOIO is enabled, but it requires WITH_CPP11 which is disabled.")
endif()

if(WITH_CPP11)
  set(MARSYAS_REALTIME ON)
  set(MARSYAS_JSON OFF)
  set(MARSYAS_SCRIPT ON)
else()
  set(MARSYAS_REALTIME OFF)
  set(MARSYAS_JSON OFF)
  set(MARSYAS_SCRIPT OFF)
  set(msg "Some MarSystems and applications will not be built")
  set(msg "${msg}, because C++11 features are disabled (CMake option WITH_CPP11)\n")
  message(WARNING "${msg}")
endif()
