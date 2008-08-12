#
# If you add a #define for .cpp code, make sure you modify
#    src/config.h.in
# as well!
#


### DETECT MAIN PACKAGES
if (MARSYAS_MACOSX)
	find_library (COREFOUNDATION_LIBRARY CoreFoundation)
	mark_as_advanced (COREFOUNDATION_LIBRARY)
	if (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
		find_library (COREAUDIO_LIBRARY CoreAudio)
		mark_as_advanced (COREAUDIO_LIBRARY)
		find_library (COREMIDI_LIBRARY CoreMidi)
		mark_as_advanced (COREMIDI_LIBRARY)
	endif (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
endif (MARSYAS_MACOSX)

if (MARSYAS_LINUX)
	if (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
    		if (OSS)
			find_package(OSS REQUIRED)
		else (OSS)
			find_package(ALSA REQUIRED)
    		endif (OSS)
	endif (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
endif (MARSYAS_LINUX)

if (MARSYAS_CYGWIN OR MARSYAS_MINGW OR MARSYAS_WIN32)
	if (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
# temporarily comment these out until I can test them.
#		find_package(DirectX REQUIRED)
	endif (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
endif (MARSYAS_CYGWIN OR MARSYAS_MINGW OR MARSYAS_WIN32)
 

### DETECT OPTIONAL PACKAGES
if (WITH_MAD)
	find_package(mad REQUIRED)
	set (MARSYAS_MAD 1)
endif (WITH_MAD)

if (WITH_VORBIS)
	find_package(OggVorbis REQUIRED)
	set (MARSYAS_VORBIS 1)
endif (WITH_VORBIS)

if (WITH_MATLAB) 
	find_package(matlab REQUIRED)
	# workaround for broken FindMatlab.cmake
	if (not MATLAB_FOUND)
		message(FATAL_ERROR "FAILED to found Matlab")
	endif (not MATLAB_FOUND)
	set (MARSYAS_MATLAB 1) 
endif (WITH_MATLAB) 

if (WITH_SWIG) 
	find_package(swig REQUIRED)
endif (WITH_SWIG) 

if (WITH_QT)
	find_package(qt4 REQUIRED)
endif (WITH_QT)

