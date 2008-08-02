FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE (${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyas/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyasqt/)

SET(marsyasqt_SOURCES
  MarSystemQtWrapper.cpp
  QtMarPlot.cpp
  Marx2DGraph.cpp
  OscMapper.cpp
  qoscclient.cpp
  qoscserver.cpp
  qosctypes.cpp
)
SET(marsyasqt_HEADERS
  MarSystemQtWrapper.h
  QtMarAbout.h
  QtMarPlot.h
  Marx2DGraph.h
  OscMapper.h
  qoscclient.h
  qoscserver.h
  qosctypes.h
)

QT4_WRAP_CPP( marsyasqt_moc_SOURCES ${marsyasqt_HEADERS})

ADD_LIBRARY(marsyasqt STATIC ${marsyasqt_SOURCES}
${marsyasqt_moc_SOURCES})
TARGET_LINK_LIBRARIES( marsyasqt ${QT_LIBRARIES} marsyas)

INSTALL(FILES ${marsyasqt_HEADERS} DESTINATION include/marsyasqt)
INSTALL(TARGETS marsyasqt LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)


