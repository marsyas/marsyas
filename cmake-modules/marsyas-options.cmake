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
option(MARSYAS_AUDIOIO "Build the audio I/O interface" ON)
option(MARSYAS_MIDIIO "Build the MIDI I/O interface" ON)
option(MARSYAS_INLINE "Inline functions" ON)
if (MARSYAS_LINUX)
	option(MARSYAS_OSS "[LINUX ONLY] Use OSS instead of ALSA" OFF)
endif (MARSYAS_LINUX)

if (MARSYAS_WIN32 OR MARSYAS_MINGW OR MARSYAS_CYGWIN)
	option(MARSYAS_ASIO "[WINDOWS ONLY] Use ASIO instead of DirectSound" OFF)
endif (MARSYAS_WIN32 OR MARSYAS_MINGW OR MARSYAS_CYGWIN)


## OPTIONAL CODE
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
option(WITH_QT "Build the QT library and applications" OFF)
option(WITH_VAMP "Build plugins for Vamp" OFF)
option(WITH_MAX/MSP "Build externals for Max/MSP" OFF)
option(WITH_PD "Build externals for Pd" OFF)
option(WITH_OSC  "Build osc support" OFF)
option(WITH_PNG  "Build png writer" OFF) 
option(WITH_OPENGL "Build programs that require OpenGL" OFF)
option(WITH_GSTREAMER "Enable use of GStreamer as audio source" OFF)
#option(WITH_QGL "Enable qglviewer (Qt-based OpenGL viewer)" OFF)
#option(WITH_READLINE "Readline functionality for Marsyas Scripting
#Language (MSL)" OFF)

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
set(MARSYAS_ASSERTS ON CACHE BOOL "" FORCE)
set(MARSYAS_FLOWCHECK ON CACHE BOOL "" FORCE)
set(MARSYAS_TRACECONTROLS ON CACHE BOOL "" FORCE)
set(MARSYAS_INLINE OFF CACHE BOOL "" FORCE)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
