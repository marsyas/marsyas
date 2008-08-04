FIND_PACKAGE(Qt4 REQUIRED) 
INCLUDE (${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyas/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/marsyasqt/)

SET(MarPlayer_SOURCES 
	MarPlayer.cpp
	Mapper.cpp 
	main.cpp
)

SET(MarPlayer_HEADERS 
	MarPlayer.h 
	Mapper.h
)


QT4_WRAP_UI(MarPlayer_UIHEADERS MarPlayerForm.ui)
QT4_WRAP_CPP(MarPlayer_moc_SOURCES ${MarPlayer_HEADERS})
QT4_ADD_RESOURCES(MarPlayerForm.qrc)
ADD_EXECUTABLE(MarPlayer ${MarPlayer_SOURCES} ${MarPlayer_UIHEADERS} ${MarPlayer_moc_SOURCES})
TARGET_LINK_LIBRARIES(MarPlayer marsyas marsyasqt ${OS_LIBS})

