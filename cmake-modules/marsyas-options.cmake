#
# If you add an option, make sure you modify
#    src/config.h.in
# as well!
#

### USER OPTIONS
option(MARSYAS_AUDIOIO "Build the audio I/O interface" ON)
option(MARSYAS_MIDIIO "Build the MIDI I/O interface" ON)
if (MARSYAS_LINUX)
	option(OSS "[LINUX ONLY] Use OSS instead of ALSA" OFF)
endif (MARSYAS_LINUX)


## code message stuff
option(MARSYAS_ASSERT "Build with assertions" OFF)
#mark_as_advanced (MARSYAS_ASSERT)
option(MARSYAS_PROFILING "Build with profiling" OFF)
#mark_as_advanced (MARSYAS_PROFILING)
option(MARSYAS_DEBUG "Build with debugging info (large performance penalty)" OFF)
#mark_as_advanced (MARSYAS_DEBUG)


## logging stuff
option(MARSYAS_LOG_WARNINGS "Build with warnings" OFF)
#mark_as_advanced (MARSYAS_LOG_WARNINGS)
option(MARSYAS_LOG_DEBUGS "Log debugs" OFF)
#mark_as_advanced (MARSYAS_LOG_DEBUGS)
option(MARSYAS_LOG_DIAGNOSTICS "Log debugs" OFF)
#mark_as_advanced (MARSYAS_LOG_DIAGNOSTICS)
option(MARSYAS_LOG2FILE "Log messages to a file" OFF)
#mark_as_advanced (MARSYAS_LOG2FILE)
option(MARSYAS_LOG2STDOUT "Log messages to a standard out" OFF)
#mark_as_advanced (MARSYAS_LOG2STDOUT)
option(MARSYAS_LOG2GUI "Log messages to GUI" OFF)
#mark_as_advanced (MARSYAS_LOG2GUI)


## using other software
option(WITH_MAD "Link to libmad (mp3 audio decoding)" OFF)
option(WITH_VORBIS "Link to libvorbis (ogg audio decoding)" OFF)
option(WITH_MATLAB "Activate MATLAB enging interface" OFF)
option(WITH_SWIG "Enable SWIG generated bindings" OFF)
option(WITH_QT "Build the QT library and applications" OFF)
#option(WITH_QGL "Enable qglviewer (Qt-based OpenGL viewer)" OFF)
#option(WITH_READLINE "Readline functionality for Marsyas Scripting
#Language (MSL)" OFF)


## other
if (MARSYAS_LINUX OR MARSYAS_MACOSX)
	option(DISTRIBUTED "[EXPERIMENTAL] compile code for distributed
		audio processing" OFF)
endif (MARSYAS_LINUX OR MARSYAS_MACOSX)




### DETECT PACKAGES

if (WITH_MAD)
	find_package(mad REQUIRED)
	set (MARSYAS_MAD 1)
endif (WITH_MAD)

if (WITH_VORBIS)
	find_package(oggvorbis REQUIRED)
	set (MARSYAS_VORBIS 1)
endif (WITH_VORBIS)

if (WITH_MATLAB) 
#	include(FindMatlab)
	find_package(matlab REQUIRED)
	set (MARSYAS_MATLAB 1) 
endif (WITH_MATLAB) 

if (WITH_SWIG) 
#	include(FindSWIG)
	find_package(swig REQUIRED)
endif (WITH_SWIG) 

if (WITH_QT)
	include(FindQt4)
endif (WITH_QT)

