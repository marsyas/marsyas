set(APP_NAME
	MarPlayer
)

set(APP_SOURCES 
	MarPlayer.cpp
	Mapper.cpp 
	main.cpp
)

set(APP_HEADERS 
	MarPlayer.h 
	Mapper.h
)

set(APP_UI
	MarPlayerForm.ui
)

set(APP_RESOURCES
	MarPlayerForm.qrc
)


# you probably don't need to change anything below this
include_directories(${CMAKE_CURRENT_BINARY_DIR})

## PRE-PROCESSING
qt4_wrap_cpp(APP_MOC_SOURCES ${APP_HEADERS})
qt4_wrap_ui(APP_UIHEADERS ${APP_UI})
qt4_add_resources(APP_RESOURCE_FILES ${APP_RESOURCES})

## COMPILING AND LINKING
add_executable(${APP_NAME} ${APP_SOURCES} ${APP_UIHEADERS}
${APP_MOC_SOURCES} ${APP_RESOURCE_FILES})
TARGET_LINK_LIBRARIES(${APP_NAME} marsyas marsyasqt ${OS_LIBS})

install(TARGETS ${APP_NAME} DESTINATION bin)

