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
		find_library (COREMIDI_LIBRARY CoreMIDI)
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
		find_package(DirectX REQUIRED)
	endif (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
endif (MARSYAS_CYGWIN OR MARSYAS_MINGW OR MARSYAS_WIN32)
 

### DETECT OPTIONAL PACKAGES
if (WITH_MAD)
	find_package(mad REQUIRED)
	set (MARSYAS_MAD 1)
endif (WITH_MAD)




if (WITH_LAME)
	find_package(lame REQUIRED)
	set (MARSYAS_LAME 1)
endif (WITH_LAME)

if (WITH_VORBIS)
	find_package(OggVorbis REQUIRED)
	set (MARSYAS_VORBIS 1)
endif (WITH_VORBIS)

if (WITH_MATLAB) 
	find_package(Matlab REQUIRED)
	# workaround for broken FindMatlab.cmake
	if (NOT MATLAB_FOUND)
		message(FATAL_ERROR "FAILED to find Matlab")
	endif (NOT MATLAB_FOUND)
	set (MARSYAS_MATLAB 1) 
endif (WITH_MATLAB) 

if (WITH_SWIG) 
	find_package(SWIG REQUIRED)
	if (WITH_SWIG_PYTHON)
		find_package(PythonInterp REQUIRED)
		find_package(PythonLibs REQUIRED)
	endif (WITH_SWIG_PYTHON)
	if (WITH_SWIG_JAVA)
		# TODO
	endif (WITH_SWIG_JAVA)
	if (WITH_SWIG_LUA)
		# TODO
	endif (WITH_SWIG_LUA)
	if (WITH_SWIG_RUBY)
		# TODO
	endif (WITH_SWIG_RUBY)
endif (WITH_SWIG) 

if (WITH_QT)
	find_package(Qt4 REQUIRED)
endif (WITH_QT)

if (WITH_VAMP)
	find_package(Vamp REQUIRED)
endif (WITH_VAMP)

if (WITH_MAX/MSP)
	find_package(MaxSDK REQUIRED)
endif (WITH_MAX/MSP)

if (WITH_PD)
	find_package(Pd REQUIRED)
endif (WITH_PD)

if (WITH_OSC) 
	set (MARSYAS_OSC 1)
endif (WITH_OSC)

if (WITH_PNG) 
	# find_package(png REQUIRED) 
   find_package(Freetype REQUIRED)
	set (MARSYAS_PNG 1)
endif (WITH_PNG)

if (WITH_ANN) 
	set (MARSYAS_ANN 1)
endif (WITH_ANN)

if (WITH_GSTREAMER)
	find_package(GObject REQUIRED)
	find_package(GLib REQUIRED)
	find_package(GStreamer REQUIRED)
	find_package(GStreamer-App REQUIRED)
	find_package(LibXml2 REQUIRED)
	set (MARSYAS_GSTREAMER 1) 
endif (WITH_GSTREAMER)

if (WITH_OPENGL)
	find_package(OpenGL REQUIRED)
	set (MARSYAS_OPENGL 1) 
	SET(QT_USE_QTOPENGL TRUE)	
	find_package(GLUT REQUIRED)
endif (WITH_OPENGL)
