set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

## setup Marsyas includes
include_directories(${CMAKE_SOURCE_DIR}/marsyas/)


if (MARSYAS_AUDIOIO) 
	include_directories(${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/)
endif (MARSYAS_AUDIOIO) 

if (MARSYAS_MIDIIO) 
	include_directories(${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/)
endif (MARSYAS_MIDIIO) 


