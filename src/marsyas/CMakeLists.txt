set( Marsyas_SOURCES
  basis.cpp
  Collection.cpp
  Color.cpp
  CommandLineOptions.cpp
  Communicator.cpp
  Conversions.cpp
  fft.cpp
  FileName.cpp
  lu.cpp
  marohtml.cpp
  marojson.cpp
  marostring.cpp
  marosvg.cpp
  maroxml.cpp
  MATLABengine.cpp
  MrsLog.cpp
  NumericLib.cpp
  peakView.cpp
  PeUtilities.cpp
  QGMMModel.cpp
  statistics.cpp
  Stk.cpp
  Thread.cpp
  TimeLine.cpp
  Transcriber.cpp
  TranscriberExtract.cpp
  vmblock.cpp
  WekaData.cpp

  core/EvEvent.cpp
  core/EvExpr.cpp
  core/EvGetUpd.cpp
  core/EvValUpd.cpp
  core/ExCommon.cpp
  core/ExNode.cpp
  core/ExParser.cpp
  core/Expr.cpp
  core/ExScanner.cpp
  core/ExSymTbl.cpp
  core/ExVal.cpp
  core/MarControl.cpp
  core/MarControlManager.cpp
  core/MarControlValue.cpp
  core/MarSystem.cpp
  core/MarSystemManager.cpp
  core/realvec.cpp
  core/Repeat.cpp
  core/Scheduler.cpp
  core/TmControlValue.cpp
  core/TmParam.cpp
  core/TmRealTime.cpp
  core/TmSampleCount.cpp
  core/TmTime.cpp
  core/TmTimer.cpp
  core/TmTimerManager.cpp
  core/TmVirtualTime.cpp
)

set( Marsyas_HEADERS
  basis.h
  Collection.h
  Color.h
  CommandLineOptions.h
  common_header.h
  common_source.h
  Communicator.h
  Conversions.h
  DLLDefines.h
  dsound.h
  ERBTools.h
  fft.h
  FileName.h
  Heap.h
  lu.h
  marohtml.h
  marojson.h
  marostring.h
  marosvg.h
  maroxml.h
  marsyas_types.h
  MATLABengine.h
  mididevices.h
  MrsLog.h
  NumericLib.h
  peakView.h
  PeUtilities.h
  QGMMModel.h
  static_vq_codebook.h
  statistics.h
  Stk.h
  StrobeList.h
  Thread.h
  TimeLine.h
  TranscriberExtract.h
  Transcriber.h
  vmblock.h
  WekaData.h

  core/EvEvent.h
  core/EvExpr.h
  core/EvGetUpd.h
  core/EvValUpd.h
  core/ExCommon.h
  core/ExNode.h
  core/ExParser.h
  core/Expr.h
  core/ExScanner.h
  core/ExSymTbl.h
  core/ExVal.h
  core/MarControlAccessor.h
  core/MarControl.h
  core/MarControlManager.h
  core/MarControlValue.h
  core/MarSystem.h
  core/MarSystemManager.h
  core/realvec.h
  core/Repeat.h
  core/Scheduler.h
  core/TmControlValue.h
  core/TmParam.h
  core/TmRealTime.h
  core/TmSampleCount.h
  core/TmTime.h
  core/TmTimer.h
  core/TmTimerManager.h
  core/TmVirtualTime.h
)

file(STRINGS marsystems/CMake-compile-cpp.txt
  MARSYAS_MARSYSTEMS_SOURCES)
file(STRINGS marsystems/CMake-compile-h.txt
  MARSYAS_MARSYSTEMS_HEADERS)


## split dirs
include_directories(core)
include_directories(marsystems)
include_directories(optional)

# this doesn't work, so I expanded it below:
#list(APPEND Marsyas_SOURCES ${MARSYAS_CORE_SOURCES})
#list(APPEND Marsyas_HEADERS ${MARSYAS_CORE_HEADERS})
# expanded version:
foreach (filename ${MARSYAS_MARSYSTEMS_SOURCES})
	list(APPEND Marsyas_SOURCES marsystems/${filename})
endforeach()
foreach (filename ${MARSYAS_MARSYSTEMS_HEADERS})
	list(APPEND Marsyas_HEADERS marsystems/${filename})
endforeach()

if(MARSYAS_AUDIOIO)
	list(APPEND Marsyas_SOURCES
		Talk.cpp
		marsystems/AudioSource.cpp
		marsystems/AudioSourceBlocking.cpp
		marsystems/AudioSink.cpp
		marsystems/AudioSinkBlocking.cpp
	)
	list(APPEND Marsyas_HEADERS
		Talk.h
		marsystems/AudioSource.h
		marsystems/AudioSourceBlocking.h
		marsystems/AudioSink.h
		marsystems/AudioSinkBlocking.h
	)
endif()

list(APPEND Marsyas_SOURCES
	realtime/runner.cpp
)

## OTHERLIBS

set( EXTERNALS_DIR ${CMAKE_SOURCE_DIR}/src/otherlibs )

list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/libsvm/svm.cpp)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/libsvm/svm.h)
list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/linear.cpp)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/linear.h)
list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/tron.cpp)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/tron.h)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/blas/blas.h)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/blas/blasp.h)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/blas/blas.h)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/blas/daxpy.c)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/blas/ddot.c)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/blas/dnrm2.c)
list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/liblinear/blas/dscal.c)


if (MARSYAS_OSC)

set( OSCPACK_DIR ${EXTERNALS_DIR}/oscpack )

list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/osc/OscException.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/osc/OscHostEndianness.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/osc/OscOutboundPacketStream.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/osc/OscPacketListener.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/osc/OscPrintReceivedElements.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/osc/OscReceivedElements.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/osc/OscTypes.h)

list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/ip/IpEndpointName.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/ip/NetworkingUtils.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/ip/PacketListener.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/ip/TimerListener.h)
list(APPEND Marsyas_HEADERS ${OSCPACK_DIR}/ip/UdpSocket.h)


list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/osc/OscOutboundPacketStream.cpp)
list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/osc/OscPrintReceivedElements.cpp)
list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/osc/OscReceivedElements.cpp)
list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/osc/OscTypes.cpp)
list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/ip/IpEndpointName.cpp)

if (MARSYAS_WIN32)
   list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/ip/win32/NetworkingUtils.cpp)
   list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/ip/win32/UdpSocket.cpp)
else (MARSYAS_WIN32)
   list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/ip/posix/NetworkingUtils.cpp)
   list(APPEND Marsyas_SOURCES ${OSCPACK_DIR}/ip/posix/UdpSocket.cpp)
endif (MARSYAS_WIN32)

endif (MARSYAS_OSC)




if (MARSYAS_PNG)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/pngwriter/pngwriter.cc)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/libpng-1.2.35/png.h)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/pngwriter/pngwriter.h)
endif (MARSYAS_PNG)


if (MARSYAS_ANN)
    set( ANN_DIR ${EXTERNALS_DIR}/ANN )
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_fix_rad_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_pr_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_tree.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/bd_tree.h)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/brute.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_dump.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_fix_rad_search.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/kd_fix_rad_search.h)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_pr_search.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/kd_pr_search.h)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_search.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/kd_search.h)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_split.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/kd_split.h)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_tree.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/kd_tree.h)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_util.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/kd_util.h)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/perf.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/pr_queue.h)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/pr_queue_k.h)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/ANN.cpp)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/ANN.h)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/ANNperf.h)
    list(APPEND Marsyas_HEADERS ${ANN_DIR}/ANNx.h)
endif (MARSYAS_ANN)

if (MARSYAS_MAD)
    list(APPEND Marsyas_SOURCES optional/MP3FileSource.cpp)
    list(APPEND Marsyas_HEADERS optional/MP3FileSource.h)
    include_directories(${mad_INCLUDE_DIR})
endif (MARSYAS_MAD)

if (MARSYAS_PNG) 
    include_directories(${FREETYPE_INCLUDE_DIR_freetype2})
	include_directories(${FREETYPE_INCLUDE_DIR_ft2build})

endif (MARSYAS_PNG)

if (MARSYAS_LAME)
    list(APPEND Marsyas_SOURCES optional/MP3FileSink.cpp)
    list(APPEND Marsyas_HEADERS optional/MP3FileSink.h)
	include_directories(${lame_INCLUDE_DIR})
endif (MARSYAS_LAME)

if (MARSYAS_VORBIS)
    list(APPEND Marsyas_SOURCES optional/OggFileSource.cpp)
    list(APPEND Marsyas_HEADERS optional/OggFileSource.h)
	include_directories(${VORBIS_INCLUDE_DIR})
endif (MARSYAS_VORBIS)

if (MARSYAS_GSTREAMER)
#	list(APPEND Marsyas_SOURCES GStreamerSource.cpp)
	list(APPEND Marsyas_SOURCES optional/GStreamerSource.cpp)
	list(APPEND Marsyas_HEADERS optional/GStreamerSource.h)
	include_directories(${GSTREAMER_INCLUDE_DIRS})
endif (MARSYAS_GSTREAMER)

if (MARSYAS_MATLAB)
#	list(APPEND Marsyas_SOURCES MATLABengine.cpp)
	include_directories(${MATLAB_INCLUDE_DIR})
endif (MARSYAS_MATLAB)  

if (MARSYAS_AUDIOIO)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/RtAudio/RtAudio.cpp)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/RtAudio/RtAudio.h)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/RtAudio/RtError.h)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/RtAudio/RtAudio3.cpp)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/RtAudio/RtAudio3.h)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/RtAudio/RtError3.h)
endif (MARSYAS_AUDIOIO)

if (MARSYAS_MIDIIO)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/RtMidi/RtMidi.cpp)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/RtMidi/RtMidi.h)
	list(APPEND Marsyas_HEADERS ${EXTERNALS_DIR}/RtMidi/RtError.h)
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

  install(TARGETS marsyas DESTINATION lib)

  if(WIN32 OR APPLE AND DEFINED PYTHON_MODULES_DIR)
    install(TARGETS marsyas DESTINATION ${PYTHON_MODULES_DIR})
  endif()
endif (MARSYAS_STATIC)
