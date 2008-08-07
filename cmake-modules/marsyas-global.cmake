set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

## setup Marsyas includes
include_directories(${CMAKE_SOURCE_DIR}/marsyas/)
include_directories(${CMAKE_SOURCE_DIR})


if (MARSYAS_AUDIOIO) 
	include_directories(${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/)
endif (MARSYAS_AUDIOIO) 

if (MARSYAS_MIDIIO) 
	include_directories(${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/)
endif (MARSYAS_MIDIIO) 


