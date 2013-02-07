## global includes
include_directories(${CMAKE_SOURCE_DIR}/marsyasqt/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/core/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/marsystems/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/optional/)


## BUILD MACRO FOR QT4APP
macro (marsyas_qt4app APP_NAME APP_SOURCES APP_HEADERS APP_UI APP_RESOURCES)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(APP_MOC_SOURCES ${APP_HEADERS})
qt4_wrap_ui(APP_UIHEADERS ${APP_UI})
qt4_add_resources(APP_RESOURCE_FILES ${APP_RESOURCES})


add_executable(${APP_NAME} ${APP_SOURCES} ${APP_UIHEADERS}
${APP_MOC_SOURCES} ${APP_RESOURCE_FILES})

## Link Libraries
#TARGET_LINK_LIBRARIES(${APP_NAME} marsyas marsyasqt ${OS_LIBS})
if (MARSYAS_OPENGL)
    TARGET_LINK_LIBRARIES(${APP_NAME} marsyas marsyasqt ${OS_LIBS} ${OPENGL_LIBRARIES})
else (MARSYAS_OPENGL)
    TARGET_LINK_LIBRARIES(${APP_NAME} marsyas marsyasqt ${OS_LIBS})
endif (MARSYAS_OPENGL)


install(TARGETS ${APP_NAME} DESTINATION bin)
endmacro (marsyas_qt4app)



## build apps
add_subdirectory(MarPlayer)
add_subdirectory(MarPhasevocoder)
add_subdirectory(MarLpc)
add_subdirectory(MarGrid)
add_subdirectory(MarGrid2)
add_subdirectory(MarNetworkWidget)
add_subdirectory(MarOscIn)
add_subdirectory(MarOscOut)
add_subdirectory(MarOscMultiGain)
add_subdirectory(MarMonitors)
add_subdirectory(Marx2DGraph)
add_subdirectory(Marx2DStackedGraphs)

if (MARSYAS_OPENGL)
	add_subdirectory(MarSndPeek)
	add_subdirectory(MarPanning)
	add_subdirectory(MarCorrelogram)
endif (MARSYAS_OPENGL)

