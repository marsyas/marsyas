FIND_PACKAGE(Qt4 REQUIRED) 
INCLUDE (${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyas/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyasqt/)

SET(MarPhasevocoder_SOURCES 
	MarPhasevocoderWindow.cpp
	MarControlGUI.cpp 
	main.cpp
)

SET(MarPhasevocoder_HEADERS 
	MarPhasevocoderWindow.h 
	MarControlGUI.h
)


QT4_WRAP_CPP(MarPhasevocoder_moc_SOURCES ${MarPhasevocoder_HEADERS})
ADD_EXECUTABLE(MarPhasevocoder ${MarPhasevocoder_SOURCES} ${MarPhasevocoder_moc_SOURCES})
TARGET_LINK_LIBRARIES(MarPhasevocoder marsyas marsyasqt ${OS_LIBS})

