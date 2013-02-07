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


## PRE-PROCESSING
qt4_wrap_cpp( marsyasqt_moc_SOURCES ${marsyasqt_HEADERS})


## INCLUDING
include_directories(${CMAKE_SOURCE_DIR}/marsyasqt/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/core/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/marsystems/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/optional/)


## BUILDING AND LINKING
add_library(marsyasqt STATIC ${marsyasqt_SOURCES} ${marsyasqt_moc_SOURCES})
target_link_libraries( marsyasqt ${QT_LIBRARIES} marsyas)


## INSTALLING
install(FILES ${marsyasqt_HEADERS} DESTINATION include/marsyasqt)
install(TARGETS marsyasqt LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)


