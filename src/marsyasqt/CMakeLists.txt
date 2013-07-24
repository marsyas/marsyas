set(marsyasqt_SOURCES
  MarSystemQtWrapper.cpp
  QtMarPlot.cpp
  Marx2DGraph.cpp
  OscMapper.cpp
  qoscclient.cpp
  qoscserver.cpp
  qosctypes.cpp
)

set(marsyasqt_HEADERS
  MarSystemQtWrapper.h
  QtMarAbout.h
  QtMarPlot.h
  Marx2DGraph.h
  OscMapper.h
  qoscclient.h
  qoscserver.h
  qosctypes.h
)

if(MARSYAS_REALTIME)
  list(APPEND marsyasqt_SOURCES marsystem_wrapper.cpp)
  list(APPEND marsyasqt_HEADERS marsystem_wrapper.h)
endif()

## PRE-PROCESSING
qt4_wrap_cpp( marsyasqt_moc_SOURCES ${marsyasqt_HEADERS})


## INCLUDING
include_directories(${CMAKE_SOURCE_DIR}/src/marsyasqt/)
include_directories(${CMAKE_SOURCE_DIR}/src/marsyas/core/)
include_directories(${CMAKE_SOURCE_DIR}/src/marsyas/marsystems/)
include_directories(${CMAKE_SOURCE_DIR}/src/marsyas/optional/)


## BUILDING AND LINKING
add_library(marsyasqt STATIC ${marsyasqt_SOURCES} ${marsyasqt_moc_SOURCES})
target_link_libraries( marsyasqt ${QT_LIBRARIES} marsyas)

set_target_properties(
	marsyasqt
	PROPERTIES COMPILE_FLAGS -std=c++11
)

## INSTALLING
install(FILES ${marsyasqt_HEADERS} DESTINATION include/marsyasqt)
install(TARGETS marsyasqt LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)


