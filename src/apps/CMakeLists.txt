## BUILD MACRO
macro (marsyas_app APP APP_SOURCES)
add_executable(${APP} ${APP_SOURCES})
include_directories(${GSTREAMER_INCLUDE_DIRS} ${PNGWRITER_INCLUDE_DIR})
if (MARSYAS_PNG) 
	target_link_libraries(${APP} marsyas png12_static zlib ${OS_LIBS})
else (MARSYAS_PNG)
	target_link_libraries(${APP} marsyas ${OS_LIBS})
endif (MARSYAS_PNG)

install(TARGETS ${APP} DESTINATION bin)
endmacro (marsyas_app)

## build apps
## in docs -gp
add_subdirectory(mkcollection)
add_subdirectory(sfplay)
add_subdirectory(sfinfo)
add_subdirectory(audioCompare)
add_subdirectory(record)
add_subdirectory(orcarecord)
add_subdirectory(pitchextract)
add_subdirectory(bextract)
add_subdirectory(phasevocoder)
add_subdirectory(kea)
add_subdirectory(peakSynth)
add_subdirectory(peakClustering)
add_subdirectory(WHaSp)
add_subdirectory(sfplugin)
add_subdirectory(mudbox)
add_subdirectory(rhythmMap)
add_subdirectory(sound2png)
add_subdirectory(ibt)
#add_subdirectory(msl) # NOT MULTIPLARTFORM (only UNIX)

## EVALUATION PROJECTS
add_subdirectory(mirex2008)
add_subdirectory(peakClusteringEval)
add_subdirectory(mirex2009_tsunoo)

## not in docs; must edit tools.texi   -gp
add_subdirectory(tempo) 
add_subdirectory(virtualsensor)
add_subdirectory(onsets)

