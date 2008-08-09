## library includes
include_directories(${CMAKE_SOURCE_DIR}/otherlibs/libsvm/)
list(APPEND MarSystem_SOURCES ../../otherlibs/libsvm/svm.cpp)

if (MARSYAS_AUDIOIO)
	list(APPEND MarSystem_SOURCES ../../otherlibs/RtAudio/RtAudio.cpp)
	list(APPEND MarSystem_SOURCES ../../otherlibs/RtAudio/RtAudio3.cpp)
endif (MARSYAS_AUDIOIO)

if (MARSYAS_MIDIIO)
	list(APPEND MarSystem_SOURCES ../../otherlibs/RtMidi/RtMidi.cpp)
endif (MARSYAS_MIDIIO)


if (MARSYAS_MAD)
	include_directories(${mad_INCLUDE_DIR})
endif (MARSYAS_MAD)

if (MARSYAS_MATLAB) 
	include_directories(${MATLAB_INCLUDE_DIR})
endif (MARSYAS_MATLAB)


## OS-specific
if (MARSYAS_MACOSX)
	if (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
		add_definitions(-D__MACOSX_CORE__)
	endif (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
endif (MARSYAS_MACOSX)

if (MARSYAS_LINUX)
	if (MARSYAS_AUDIOIO)
    		if (OSS)
			add_definitions(-D__LINUX_OSS__)
		else (OSS)
			add_definitions(-D__LINUX_ALSA__)
    		endif (OSS)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		if (OSS)
			add_definitions(-D__LINUX_OSS__)
		else (OSS)
			add_definitions(-D__LINUX_ALSASEQ__)
    		endif (OSS)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_LINUX)

if (MARSYAS_CYGWIN)
	if (MARSYAS_AUDIOIO)
		add_definitions(-D__WINDOWS_DS__)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_CYGWIN)

if (MARSYAS_MINGW)
	if (MARSYAS_AUDIOIO)
		add_definitions(-D__WINDOWS_DS__)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_MINGW)
 
if (MARSYAS_WIN32)
	if (MARSYAS_AUDIOIO)
		add_definitions(-D__WINDOWS_DS__)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_WIN32)




