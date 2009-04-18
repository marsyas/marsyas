#
# If you add a #define for .cpp code, make sure you modify
#    src/config.h.in
# as well!
#

### USER OPTIONS
option(MARSYAS_AUDIOIO "Build the audio I/O interface" ON)
option(MARSYAS_MIDIIO "Build the MIDI I/O interface" ON)
if (MARSYAS_LINUX)
	option(OSS "[LINUX ONLY] Use OSS instead of ALSA" OFF)
endif (MARSYAS_LINUX)

if (MARSYAS_WIN32 OR MARSYAS_MINGW OR MARSYAS_CYGWIN)
	option(ASIO "[WINDOWS ONLY] Use ASIO instead of DirectSound" OFF)
endif (MARSYAS_WIN32 OR MARSYAS_MINGW OR MARSYAS_CYGWIN)


## OPTIONAL CODE
option(BUILD_APPS "Build the applications" ON)
mark_as_advanced (BUILD_APPS)
option(MARSYAS_TESTS "Build the tests" OFF)
if (MARSYAS_LINUX OR MARSYAS_MACOSX)
	option(BUILD_DISTRIBUTED "[EXPERIMENTAL] compile code for
		distributed audio processing" OFF)
	mark_as_advanced(BUILD_DISTRIBUTED)
	if (BUILD_DISTRIBUTED)
		set(DISTRIBUTED 1)
	endif (BUILD_DISTRIBUTED)
endif (MARSYAS_LINUX OR MARSYAS_MACOSX)


## CODE MESSAGES
option(MARSYAS_ASSERTS "Build with assertions" OFF)
option(MARSYAS_PROFILING "Build with profiling" OFF)
#mark_as_advanced (MARSYAS_PROFILING)
option(MARSYAS_DEBUG "Build with debugging info (large performance penalty)" OFF)
#mark_as_advanced (MARSYAS_DEBUG)


## logging stuff
option(MARSYAS_LOG_WARNINGS "Build with warnings" ON)
option(MARSYAS_LOG_DEBUGS "Log debugs" OFF)
option(MARSYAS_LOG_DIAGNOSTICS "Log debugs" OFF)
option(MARSYAS_LOG2FILE "Log messages to a file" OFF)
option(MARSYAS_LOG2STDOUT "Log messages to a standard out" ON)
option(MARSYAS_LOG2GUI "Log messages to GUI" OFF)
mark_as_advanced (MARSYAS_LOG2GUI)


## using other software
option(WITH_MAD "Link to libmad (mp3 audio decoding)" OFF)
option(WITH_VORBIS "Link to libvorbis (ogg audio decoding)" OFF)
option(WITH_MATLAB "Activate MATLAB enging interface" OFF)
option(WITH_SWIG "Enable SWIG generated bindings" OFF)
option(WITH_QT "Build the QT library and applications" OFF)
option(WITH_VAMP "Build plugins for Vamp" OFF)
option(WITH_PNG  "Build png writer" OFF) 
option(WITH_GSTREAMER "Enable use of GStreamer as audio source" OFF)
mark_as_advanced(WITH_GSTREAMER)
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

