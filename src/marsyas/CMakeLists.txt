set( Marsyas_SOURCES
  system/MarControl.cpp
  system/MarControlManager.cpp
  system/MarControlValue.cpp
  system/MarSystem.cpp
  system/MarSystemManager.cpp

  sched/TmTimerManager.cpp
  sched/TmRealTime.cpp
  sched/TmParam.cpp
  sched/TmControlValue.cpp
  sched/TmSampleCount.cpp
  sched/EvGetUpd.cpp
  sched/TmTime.cpp
  sched/TmVirtualTime.cpp
  sched/Repeat.cpp
  sched/TmTimer.cpp
  sched/EvExpr.cpp
  sched/EvValUpd.cpp
  sched/EvEvent.cpp
  sched/Scheduler.cpp

  expr/ExSymTbl.cpp
  expr/Expr.cpp
  expr/ExScanner.cpp
  expr/ExNode.cpp
  expr/ExCommon.cpp
  expr/ExVal.cpp
  expr/ExParser.cpp

  realvec.cpp
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
)

if(MARSYAS_REALTIME)
  set(MARSYAS_REALTIME_SOURCES
    realtime/runner.cpp
    realtime/osc_receiver.cpp
    realtime/osc_transmitter.cpp
    realtime/udp_receiver.cpp
    realtime/udp_transmitter.cpp
    realtime/controller.cpp
  )
endif()

if(MARSYAS_SCRIPT)
  if(MARSYAS_REGENERATE_SCRIPT_PARSER)
    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/script/lex.cc"
      COMMAND flexc++ ARGS scanner.l
      DEPENDS script/scanner.l
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/script"
      COMMENT "Generating Marsyas script lexer"
    )

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/script/parse.cc"
      COMMAND bisonc++ ARGS parser.y
      DEPENDS script/parser.y
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/script"
      COMMENT "Generating Marsyas script parser"
    )
  endif()

  list(APPEND Marsyas_SOURCES
    script/lex.cc
    script/parse.cc
    script/script.cpp
    script/manager.cpp
    script/operation_processor.cpp
  )

  set(parser_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/script/scanner_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/script/scanner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/script/scannerbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/script/parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/script/parser_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/script/parserbase.h
  )

  set_source_files_properties(
    script/lex.cc
    script/parse.cc
    PROPERTIES OBJECT_DEPENDS "${parser_headers}"
  )

endif()

if(WITH_CPP11)
  set(MARSYAS_DEBUG_SOURCES
    debug/recorder.cpp
    debug/debugger.cpp
    debug/file_io.cpp
  )
endif()

file(STRINGS marsystems/CMake-compile-cpp.txt
  MARSYAS_MARSYSTEMS_SOURCES)

foreach (filename ${MARSYAS_MARSYSTEMS_SOURCES})
	list(APPEND Marsyas_SOURCES marsystems/${filename})
endforeach()

if(MARSYAS_AUDIOIO)
	list(APPEND Marsyas_SOURCES
		Talk.cpp
		marsystems/AudioSource.cpp
		marsystems/AudioSourceBlocking.cpp
		marsystems/AudioSink.cpp
		marsystems/AudioSinkBlocking.cpp
	)
endif()

## OTHERLIBS

set( EXTERNALS_DIR ${CMAKE_SOURCE_DIR}/src/otherlibs )

if(MARSYAS_JSON)
  include_directories( ${EXTERNALS_DIR}/json++ )
  list(APPEND Marsyas_SOURCES json_io.cpp)
endif()

list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/libsvm/svm.cpp)
list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/linear.cpp)
list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/tron.cpp)

list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/blas/daxpy.c)
list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/blas/ddot.c)
list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/blas/dnrm2.c)
list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/liblinear/blas/dscal.c)


if (MARSYAS_PNG)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/pngwriter/pngwriter.cc)
endif (MARSYAS_PNG)


if (MARSYAS_ANN)
    set( ANN_DIR ${EXTERNALS_DIR}/ANN )
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_fix_rad_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_pr_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/bd_tree.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/brute.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_dump.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_fix_rad_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_pr_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_search.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_split.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_tree.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/kd_util.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/perf.cpp)
    list(APPEND Marsyas_SOURCES ${ANN_DIR}/ANN.cpp)
endif (MARSYAS_ANN)

if (MARSYAS_MAD)
    list(APPEND Marsyas_SOURCES marsystems/MP3FileSource.cpp)
    include_directories(${mad_INCLUDE_DIR})
endif (MARSYAS_MAD)

if (MARSYAS_PNG) 
    include_directories(${FREETYPE_INCLUDE_DIR_freetype2})
	include_directories(${FREETYPE_INCLUDE_DIR_ft2build})

endif (MARSYAS_PNG)

if (MARSYAS_LAME)
  list(APPEND Marsyas_SOURCES marsystems/MP3FileSink.cpp)
	include_directories(${lame_INCLUDE_DIR})
endif (MARSYAS_LAME)

if (MARSYAS_VORBIS)
    list(APPEND Marsyas_SOURCES marsystems/OggFileSource.cpp)
	include_directories(${VORBIS_INCLUDE_DIR})
endif (MARSYAS_VORBIS)

if (MARSYAS_GSTREAMER)
	list(APPEND Marsyas_SOURCES marsystems/GStreamerSource.cpp)
	include_directories(${GSTREAMER_INCLUDE_DIRS})
endif (MARSYAS_GSTREAMER)

if (MARSYAS_MATLAB)
#	list(APPEND Marsyas_SOURCES MATLABengine.cpp)
	include_directories(${MATLAB_INCLUDE_DIR})
endif (MARSYAS_MATLAB)  

if (MARSYAS_AUDIOIO)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/RtAudio/RtAudio.cpp)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/RtAudio/RtAudio3.cpp)
endif (MARSYAS_AUDIOIO)

if (MARSYAS_MIDIIO)
	list(APPEND Marsyas_SOURCES ${EXTERNALS_DIR}/RtMidi/RtMidi.cpp)
endif (MARSYAS_MIDIIO)


## DEFINE TARGET

set(MARSYAS_ALL_SOURCES
    ${Marsyas_SOURCES}
    ${MARSYAS_REALTIME_SOURCES}
    ${MARSYAS_DEBUG_SOURCES}
)

if(MARSYAS_STATIC)
  add_library(marsyas STATIC ${MARSYAS_ALL_SOURCES})
  set_target_properties(marsyas PROPERTIES OUTPUT_NAME "marsyas")
  set_target_properties(marsyas PROPERTIES CLEAN_DIRECT_OUTPUT 1)
else()
  add_library(marsyas SHARED ${MARSYAS_ALL_SOURCES})
endif()


## LINK

target_link_libraries(marsyas ${OS_LIBS})

if(MARSYAS_JSON)
  target_link_libraries(marsyas json++)
endif()

if(MARSYAS_OSC)
  target_link_libraries(marsyas ${OSCPACK_LIBRARIES})
endif()

if(MARSYAS_MAD)
  target_link_libraries(marsyas ${mad_LIBRARY})
endif(MARSYAS_MAD)

if(MARSYAS_PNG)
  if(MARSYAS_STATIC)
    target_link_libraries(marsyas png12_static)
  else()
    target_link_libraries(marsyas png12 ${FREETYPE_LIBRARY})
  endif()
  target_link_libraries(marsyas zlib)
endif(MARSYAS_PNG)

if(MARSYAS_LAME)
  target_link_libraries(marsyas ${lame_LIBRARY})
endif(MARSYAS_LAME)

if(MARSYAS_VORBIS)
  target_link_libraries(marsyas ${OGGVORBIS_LIBRARIES})
endif(MARSYAS_VORBIS)

if(MARSYAS_MATLAB)
  target_link_libraries(marsyas ${MATLAB_LIBRARIES})
endif(MARSYAS_MATLAB)

if(MARSYAS_GSTREAMER)
  target_link_libraries(marsyas ${GStreamer_LIBRARY})
  target_link_libraries(marsyas ${GStreamer-App_LIBRARY})
  target_link_libraries(marsyas ${Glib_LIBRARY})
  target_link_libraries(marsyas ${_GObjectLibs})
endif(MARSYAS_GSTREAMER)


## INSTALL

install( FILES
  basis.h
  Collection.h
  Color.h
  CommandLineOptions.h
  common_header.h
  common_source.h
  Communicator.h
  Conversions.h
  ${CMAKE_CURRENT_BINARY_DIR}/export.h
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
  types.h
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
  realvec.h

  DESTINATION include/marsyas
)

if(MARSYAS_JSON)
  install( FILES json_io.h DESTINATION include/marsyas )
endif()

install( FILES
  system/MarControlAccessor.h
  system/MarControl.h
  system/MarControlManager.h
  system/MarControlValue.h
  system/MarSystem.h
  system/MarSystemManager.h

  DESTINATION include/marsyas/system
)

install( FILES
  sched/EvEvent.h
  sched/EvExpr.h
  sched/EvGetUpd.h
  sched/EvValUpd.h
  sched/Repeat.h
  sched/Scheduler.h
  sched/TmControlValue.h
  sched/TmParam.h
  sched/TmRealTime.h
  sched/TmSampleCount.h
  sched/TmTime.h
  sched/TmTimer.h
  sched/TmTimerManager.h
  sched/TmVirtualTime.h

  DESTINATION include/marsyas/sched
)

install( FILES
  expr/ExCommon.h
  expr/ExNode.h
  expr/ExParser.h
  expr/Expr.h
  expr/ExScanner.h
  expr/ExSymTbl.h
  expr/ExVal.h

  DESTINATION include/marsyas/expr
)

install( FILES
  realtime/any.h
  realtime/stage.h
  realtime/queue.h
  realtime/packet_queue.h
  realtime/realvec_queue.h
  realtime/atomic_control.h
  realtime/controller.h
  realtime/osc_receiver.h
  realtime/osc_transmitter.h
  realtime/udp_receiver.h
  realtime/udp_transmitter.h
  realtime/runner.h

  DESTINATION include/marsyas/realtime
)

install( FILES
  debug/debugger.h
  debug/record.h
  debug/recorder.h
  debug/file_io.h

  DESTINATION include/marsyas/debug
)

install( FILES
  script/script.h
  script/manager.h

  DESTINATION include/marsyas/script
)

# FIXME: Install headers for MarSystem subclasses
# after they are cleaned-up and good for public.

if (MARSYAS_STATIC)
  install(TARGETS marsyas
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
else (MARSYAS_STATIC)
  install(TARGETS marsyas
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )

  if(WIN32 OR APPLE AND DEFINED PYTHON_MODULES_DIR)
    install(TARGETS marsyas
      RUNTIME DESTINATION ${PYTHON_MODULES_DIR}
      LIBRARY DESTINATION ${PYTHON_MODULES_DIR}
    )
  endif()
endif (MARSYAS_STATIC)
