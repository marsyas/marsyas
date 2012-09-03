file(GLOB Marsyas_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/marsyas *.cpp)
file(GLOB Marsyas_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/marsyas *.h)

## OTHERLIBS

list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/libsvm/svm.cpp)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/libsvm/svm.h)




if (MARSYAS_OSC)

list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscException.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscHostEndianness.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscOutboundPacketStream.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscPacketListener.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscPrintReceivedElements.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscReceivedElements.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscTypes.h)

list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/IpEndpointName.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/NetworkingUtils.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/PacketListener.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/TimerListener.h)
list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/UdpSocket.h)


list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscOutboundPacketStream.cpp)
list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscPrintReceivedElements.cpp)
list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscReceivedElements.cpp)
list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/osc/OscTypes.cpp)
list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/IpEndpointName.cpp)

if (MARSYAS_WIN32)
   list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/win32/NetworkingUtils.cpp)
   list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/win32/UdpSocket.cpp)
else (MARSYAS_WIN32)
   list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/posix/NetworkingUtils.cpp)
   list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/oscpack/ip/posix/UdpSocket.cpp)
endif (MARSYAS_WIN32)

endif (MARSYAS_OSC)




if (MARSYAS_PNG)
	list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/pngwriter/pngwriter.cc) 
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/libpng-1.2.35/png.h)
	list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/pngwriter/pngwriter.h)
endif (MARSYAS_PNG)


if (MARSYAS_ANN)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/bd_fix_rad_search.cpp)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/bd_pr_search.cpp)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/bd_search.cpp)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/bd_tree.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/bd_tree.h)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/brute.cpp)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_dump.cpp)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_fix_rad_search.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_fix_rad_search.h)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_pr_search.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_pr_search.h)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_search.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_search.h)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_split.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_split.h)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_tree.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_tree.h)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_util.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/kd_util.h)
    list(APPEND Marsyas_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/ANN/perf.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/pr_queue.h)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/pr_queue_k.h)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/ANN.cpp)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/ANN.h)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/ANNperf.h)
    list(APPEND Marsyas_HEADERS ${CMAKE_SOURCE_DIR}/otherlibs/ANN/ANNx.h)
endif (MARSYAS_ANN)

if (MARSYAS_MAD)
	include_directories(${mad_INCLUDE_DIR})
endif (MARSYAS_MAD)

if (MARSYAS_PNG) 
    include_directories(${FREETYPE_INCLUDE_DIR_freetype2})
	include_directories(${FREETYPE_INCLUDE_DIR_ft2build})

endif (MARSYAS_PNG)

if (MARSYAS_LAME)
	include_directories(${lame_INCLUDE_DIR})
endif (MARSYAS_LAME)

if (MARSYAS_VORBIS)
	include_directories(${VORBIS_INCLUDE_DIR})
endif (MARSYAS_VORBIS)

if (MARSYAS_GSTREAMER)
#	list(APPEND Marsyas_SOURCES GStreamerSource.cpp)
	include_directories(${GSTREAMER_INCLUDE_DIRS})
endif (MARSYAS_GSTREAMER)

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

if (MARSYAS_STATIC)
	add_library(marsyas_static STATIC ${Marsyas_SOURCES} ${Marsyas_HEADERS})
	SET_TARGET_PROPERTIES(marsyas_static PROPERTIES OUTPUT_NAME "marsyas")
	SET_TARGET_PROPERTIES(marsyas_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	TARGET_LINK_LIBRARIES(marsyas_static ${OS_LIBS})

## LINKING
if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(marsyas_static ${mad_LIBRARY})
endif (MARSYAS_MAD)


if (MARSYAS_PNG)
  TARGET_LINK_LIBRARIES(marsyas_static png12_static zlib)
endif (MARSYAS_PNG)

if (MARSYAS_LAME)
	TARGET_LINK_LIBRARIES(marsyas_static ${lame_LIBRARY})
endif (MARSYAS_LAME)


if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(marsyas_static ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(marsyas_static ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (MARSYAS_GSTREAMER)
	TARGET_LINK_LIBRARIES(marsyas_static ${GStreamer_LIBRARY})
	TARGET_LINK_LIBRARIES(marsyas_static  ${GStramer-App_LIBRARY})
	TARGET_LINK_LIBRARIES(marsyas_static  ${Glib_LIBRARY})
	TARGET_LINK_LIBRARIES(marsyas_static  ${_GObjectLibs})
endif (MARSYAS_GSTREAMER)


else (MARSYAS_STATIC)
     add_library(marsyas SHARED ${Marsyas_SOURCES} ${Marsyas_HEADERS})
     TARGET_LINK_LIBRARIES(marsyas ${OS_LIBS})

## LINKING
if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(marsyas ${mad_LIBRARY})
endif (MARSYAS_MAD)


if (MARSYAS_PNG)
   TARGET_LINK_LIBRARIES(marsyas png12 zlib ${FREETYPE_LIBRARY})
endif (MARSYAS_PNG)

if (MARSYAS_LAME)
	TARGET_LINK_LIBRARIES(marsyas ${lame_LIBRARY})
endif (MARSYAS_LAME)


if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(marsyas ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(marsyas ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (MARSYAS_GSTREAMER)
	TARGET_LINK_LIBRARIES(marsyas ${GStreamer_LIBRARY})
	TARGET_LINK_LIBRARIES(marsyas ${GStreamer-App_LIBRARY})
	TARGET_LINK_LIBRARIES(marsyas ${Glib_LIBRARY})
	TARGET_LINK_LIBRARIES(marsyas ${_GObjectLibs})
endif (MARSYAS_GSTREAMER)



endif (MARSYAS_STATIC)









## INSTALLING

install(FILES ${Marsyas_HEADERS} DESTINATION include/marsyas)
if (MARSYAS_STATIC)
  install(TARGETS marsyas_static LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
else (MARSYAS_STATIC)
  if (DEFINED CMAKE_INSTALL_PREFIX)
    set(LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
  else ()
    set(LIB_DIR "")
  endif ()
  set_target_properties(marsyas PROPERTIES INSTALL_NAME_DIR ${LIB_DIR})
  install(TARGETS marsyas LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
  install(TARGETS marsyas LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib)
  install(TARGETS marsyas LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
  if (DEFINED PYTHON_INSTDIR)
    install(TARGETS marsyas LIBRARY DESTINATION lib
    RUNTIME DESTINATION ${PYTHON_INSTDIR})
  endif ()
endif (MARSYAS_STATIC)
