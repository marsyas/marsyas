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
		if (ASIO)
			add_definitions(-D__WINDOWS_ASIO__)
		else (ASIO)
			add_definitions(-D__WINDOWS_DS__)
		endif (ASIO)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_CYGWIN)

if (MARSYAS_MINGW)
	if (MARSYAS_AUDIOIO)
		if (ASIO)
			add_definitions(-D__WINDOWS_ASIO__)
		else (ASIO)
			add_definitions(-D__WINDOWS_DS__)
		endif (ASIO)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_MINGW)
 
if (MARSYAS_WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	if (MARSYAS_AUDIOIO)
		if (ASIO)
			add_definitions(-D__WINDOWS_ASIO__)
		else (ASIO)
			add_definitions(-D__WINDOWS_DS__)
		endif (ASIO)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_WIN32)


## setup Marsyas includes
include_directories(${CMAKE_SOURCE_DIR}/marsyas/)

if (MARSYAS_AUDIOIO) 
	include_directories(${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/)
endif (MARSYAS_AUDIOIO) 

if (MARSYAS_MIDIIO) 
	include_directories(${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/)
endif (MARSYAS_MIDIIO) 


## from user options
if (MARSYAS_MATLAB) 
	include_directories(${MATLAB_INCLUDE_DIR})
	include_directories(${CMAKE_SOURCE_DIR}/marsyas/MATLAB)
endif (MARSYAS_MATLAB)

if (MARSYAS_PNG)
	include_directories(${CMAKE_SOURCE_DIR}/otherlibs/pngwriter/)
endif (MARSYAS_PNG)

if (MARSYAS_OPENGL)
	include_directories(${OPENGL_INCLUDE_DIR})
endif (MARSYAS_OPENGL)

## basic library includes
include_directories(${CMAKE_SOURCE_DIR}/otherlibs/libsvm/)



