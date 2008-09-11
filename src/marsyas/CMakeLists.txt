file(GLOB Marsyas_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/marsyas *.cpp)
file(GLOB Marsyas_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/marsyas *.h)

## OTHERLIBS

list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/libsvm/svm.cpp)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/libsvm/svm.h)

if (MARSYAS_MAD)
	include_directories(${mad_INCLUDE_DIR})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	include_directories(${VORBIS_INCLUDE_DIR})
endif (MARSYAS_VORBIS)

if (GSTREAMER_FOUND)
#	list(APPEND Marsyas_SOURCES GStreamerSource.cpp gst-decode.cpp)
	include_directories(${GSTREAMER_INCLUDE_DIRS})
endif (GSTREAMER_FOUND)

if (MARSYAS_MATLAB)
#	list(APPEND Marsyas_SOURCES MATLABengine.cpp)
	include_directories(${MATLAB_INCLUDE_DIR})
endif (MARSYAS_MATLAB)  

if (MARSYAS_AUDIOIO)
	list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtAudio.cpp)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtAudio.h)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtError.h)
	list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtAudio3.cpp)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtAudio3.h)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtError3.h)
endif (MARSYAS_AUDIOIO)

if (MARSYAS_MIDIIO)
	list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/RtMidi.cpp)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/RtMidi.h)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/RtError.h)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/RtError3.h)
endif (MARSYAS_MIDIIO)


## ACTUAL BUILD
add_library(marsyas STATIC ${Marsyas_SOURCES} ${Marsyas_HEADERS})

## LINKING
if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(marsyas ${mad_LIBRARY})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(marsyas ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(marsyas ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (GSTREAMER_FOUND)
	TARGET_LINK_LIBRARIES(marsyas ${GSTREAMER_LIBRARIES})
endif (GSTREAMER_FOUND)


## INSTALLING
install(FILES ${Marsyas_HEADERS} DESTINATION include/marsyas)
install(TARGETS marsyas LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

