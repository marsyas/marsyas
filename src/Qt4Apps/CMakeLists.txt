## global includes
include_directories(${CMAKE_SOURCE_DIR}/marsyasqt/)


## BUILD MACRO FOR QT4APP
macro (marsyas_qt4app APP_NAME APP_SOURCES APP_HEADERS APP_UI APP_RESOURCES)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(APP_MOC_SOURCES ${APP_HEADERS})
qt4_wrap_ui(APP_UIHEADERS ${APP_UI})
qt4_add_resources(APP_RESOURCE_FILES ${APP_RESOURCES})

add_executable(${APP_NAME} ${APP_SOURCES} ${APP_UIHEADERS}
${APP_MOC_SOURCES} ${APP_RESOURCE_FILES})
TARGET_LINK_LIBRARIES(${APP_NAME} marsyas marsyasqt ${OS_LIBS})

install(TARGETS ${APP_NAME} DESTINATION bin)
endmacro (marsyas_qt4app)



## build apps
add_subdirectory(MarPlayer)
add_subdirectory(MarPhasevocoder)
add_subdirectory(MarLpc)
