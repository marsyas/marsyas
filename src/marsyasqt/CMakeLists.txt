FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE (${QT_USE_FILE})

#MESSAGE (${CMAKE_SOURCE_DIR}/marsyas/)
#BREAK()

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyas/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyasqt/)

#INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
#INCLUDE_DIRECTORIES(.)

SET(MarsyasQt_SOURCES
  MarSystemQtWrapper.cpp
  QtMarPlot.cpp
  Marx2DGraph.cpp
  OscMapper.cpp
  qoscclient.cpp
  qoscserver.cpp
  qosctypes.cpp
)
SET(MarsyasQt_HEADERS
  MarSystemQtWrapper.h
  QtMarAbout.h
  QtMarPlot.h
  Marx2DGraph.h
  OscMapper.h
  qoscclient.h
  qoscserver.h
  qosctypes.h
)

QT4_WRAP_CPP( MarsyasQt_moc_SOURCES ${MarsyasQt_HEADERS})

ADD_LIBRARY(MarsyasQt STATIC ${MarsyasQt_SOURCES}
${MarsyasQt_moc_SOURCES})
TARGET_LINK_LIBRARIES( MarsyasQt ${QT_LIBRARIES} Marsyas)

INSTALL(FILES ${MarsyasQt_HEADERS} DESTINATION include/marsyasqt)
INSTALL(TARGETS MarsyasQt LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)


