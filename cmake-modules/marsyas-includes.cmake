## OS-specific

# The preprocessor definitions like "__MACOSX_CORE__" or "__LINUX_JACK__"
# are required for building RtAudio

if (MARSYAS_MACOSX)
  if (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
    add_definitions(-D__MACOSX_CORE__)
  endif (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
endif (MARSYAS_MACOSX)

if(MARSYAS_LINUX)
  if(WITH_JACK AND JACK_FOUND)
    include_directories(${JACK_INCLUDE_DIRS})
    add_definitions(-D__UNIX_JACK__ -D__LINUX_JACK__)
  endif()
  if(WITH_ALSA AND ALSA_FOUND)
    include_directories(${ALSA_INCLUDE_DIR})
    add_definitions(-D__LINUX_ALSA__)
  endif()
  if(WITH_OSS AND OSS_FOUND)
    include_directories(${OSS_INCLUDE_DIR})
    add_definitions(-D__LINUX_OSS__)
  endif()
endif()

if(MARSYAS_WIN32 OR MARSYAS_MINGW OR MARSYAS_CYGWIN)
  if(MARSYAS_WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  endif()
  if(MARSYAS_AUDIOIO)
    if(MARSYAS_ASIO)
      add_definitions(-D__WINDOWS_ASIO__)
    else()
      include_directories(${DSOUND_INCLUDE_DIR})
      add_definitions(-D__WINDOWS_DS__)
    endif()
  endif()
  if(MARSYAS_MIDIIO)
    add_definitions(-D__WINDOWS_MM__)
  endif()
endif()


## setup Marsyas includes

if (MARSYAS_AUDIOIO)
  include_directories(${CMAKE_SOURCE_DIR}/src/otherlibs/RtAudio/)
endif (MARSYAS_AUDIOIO)

if (MARSYAS_MIDIIO)
  include_directories(${CMAKE_SOURCE_DIR}/src/otherlibs/RtMidi/)
endif (MARSYAS_MIDIIO)

if (WITH_ANN)
  include_directories(${CMAKE_SOURCE_DIR}/src/otherlibs/ANN/)
endif (WITH_ANN)

## from user options
if (MARSYAS_MATLAB)
  include_directories(${MATLAB_INCLUDE_DIR})
  include_directories(${CMAKE_SOURCE_DIR}/src/marsyas/MATLAB)
endif (MARSYAS_MATLAB)

if (MARSYAS_OSC)
  include_directories(${OSCPACK_INCLUDE_DIRS})
endif (MARSYAS_OSC)

if (MARSYAS_PNG)
  include_directories(${CMAKE_SOURCE_DIR}/src/otherlibs/libpng-1.2.35)
  include_directories(${CMAKE_SOURCE_DIR}/src/otherlibs/pngwriter/)
endif (MARSYAS_PNG)

if (MARSYAS_OPENGL)
  include_directories(${OPENGL_INCLUDE_DIR})
  include_directories(${GLUT_INCLUDE_DIRS})
endif (MARSYAS_OPENGL)


if (MARSYAS_GSTREAMER)
  include_directories(${GStreamer_INCLUDE_DIR})
  include_directories(${Glib_INCLUDE_DIR})
  include_directories(${GlibConfig_INCLUDE_DIR})
  include_directories(${LIBXML2_INCLUDE_DIR})
endif (MARSYAS_GSTREAMER)

## basic library includes
include_directories(${CMAKE_SOURCE_DIR}/src/otherlibs/libsvm/)
#include_directories(${CMAKE_SOURCE_DIR}/src/otherlibs/liblinear/)


