include_directories(${CMAKE_SOURCE_DIR}/marsyas/core/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/marsystems/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/optional/)


## BUILD MACRO

macro (marsyas_static_app APP APP_SOURCES)
   	 add_executable(${APP} ${APP_SOURCES})
	 include_directories(${GSTREAMER_INCLUDE_DIRS}
   	 ${PNGWRITER_INCLUDE_DIR} ${OSC_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR_freetype2}
   	 ${FREETYPE_INCLUDE_DIR_ft2build} )
	 target_link_libraries(${APP} marsyas_static)
	 install(TARGETS ${APP} DESTINATION bin)
endmacro (marsyas_static_app)

macro (marsyas_app APP APP_SOURCES)
   	 add_executable(${APP} ${APP_SOURCES})
	 include_directories(${GSTREAMER_INCLUDE_DIRS}
   	 ${PNGWRITER_INCLUDE_DIR} ${OSC_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR_freetype2} ${FREETYPE_INCLUDE_DIR_ft2build})
	 if (MARSYAS_STATIC)
	 	 target_link_libraries(${APP} marsyas_static)
	 else (MARSYAS_STATIC)
	  	 target_link_libraries(${APP} marsyas)
	  endif (MARSYAS_STATIC)	      
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
add_subdirectory(peakClustering2)
add_subdirectory(WHaSp)
add_subdirectory(sfplugin)
add_subdirectory(mudbox)
add_subdirectory(rhythmMap)
add_subdirectory(sound2png)
add_subdirectory(sound2sound)
add_subdirectory(ibt)
add_subdirectory(pitchdtw)
add_subdirectory(speakerSeg)
add_subdirectory(wreckBeach)

if (MARSYAS_OPENGL)
	add_subdirectory(helloglut)
	add_subdirectory(carfac)
endif (MARSYAS_OPENGL)

#add_subdirectory(msl) # NOT MULTIPLARTFORM (only UNIX)

## EVALUATION PROJECTS
add_subdirectory(mirex2008)
add_subdirectory(mirex2010)
add_subdirectory(peakClusteringEval)
add_subdirectory(mirex2009_tsunoo)
add_subdirectory(aim)

## not in docs; must edit tools.texi   -gp
add_subdirectory(tempo) 
add_subdirectory(virtualsensor)
add_subdirectory(onsets)
add_subdirectory(helloWorld)

