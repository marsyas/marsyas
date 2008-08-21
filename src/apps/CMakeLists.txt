## setup OS_LIBS
include(marsyas-osincludes)
include(marsyas-oslibs)


## BUILD MACRO
macro (marsyas_app APP APP_SOURCES)
add_executable(${APP} ${APP_SOURCES})
include_directories(${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(${APP} marsyas ${GSTREAMER_LIBRARIES} ${OS_LIBS})
install(TARGETS ${APP} DESTINATION bin)
endmacro (marsyas_app)


## build apps
add_subdirectory(sfplay)
add_subdirectory(bextract)
add_subdirectory(sfinfo)
add_subdirectory(phasevocoder)
add_subdirectory(mudbox)
add_subdirectory(sfplugin)
add_subdirectory(tempo) 
add_subdirectory(virtualsensor)
add_subdirectory(audioCompare)
add_subdirectory(mkcollection)
add_subdirectory(peakClustering)
add_subdirectory(kea)
