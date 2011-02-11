project(ibt~)

if(MARSYAS_WIN32)
# On Windows we need to include module definition (.def) file as well
set (MarMax_SOURCES
	MarMax~.cpp
	IBT~.cpp
	ibt~.def 
)
elseif(MARSYAS_MACOSX)
set (MarMax_SOURCES
	MarMax~.cpp
	IBT~.cpp
)
endif(MARSYAS_WIN32)

set (MarMax_HEADERS
	MarMax~.h
	IBT~.h
)

#add_library(def_module MODULE IBT~.def)
add_library(ibt~ SHARED ${MarMax_SOURCES} ${MarMax_HEADERS})

include_directories(${MAXSDK_MAX_INCLUDE_DIR})
include_directories(${MAXSDK_MSP_INCLUDE_DIR})

# (for now) only works on the static version of Marsyas
# we need to link to that specific archive library. 
# Also linking some additional libs/frameworks
if (MARSYAS_STATIC)

	# CHECK & TEST
	if(MARSYAS_MACOSX)
	
		TARGET_LINK_LIBRARIES(ibt~ ${LIBRARY_OUTPUT_PATH}/libmarsyas.a)
		find_library(COREAUDIO CoreAudio "/System/Library/Frameworks")
		find_library(CARBON Carbon "/System/Library/Frameworks")
		TARGET_LINK_LIBRARIES(ibt~ ${COREAUDIO} ${CARBON})
		#Pre-processor defines
		ADD_DEFINITIONS(-DMAC_VERSION)
	
	#No Max/MSP on Linux (?)
	#elseif(MARSYAS_LINUX)
	
	# TESTED ON WINDOWS
	elseif(MARSYAS_WIN32)
	
		TARGET_LINK_LIBRARIES(ibt~ marsyas_static)
		#Pre-processor defines
		ADD_DEFINITIONS(-DWIN_VERSION -DEXT_WIN_VERSION)

	endif(MARSYAS_MACOSX)

#display warning if Marsyas not in STATIC mode (only works if statically linked to Marsyas lib)
else (MARSYAS_STATIC)
	 
	 message(SEND_ERROR "Only working linked statically to Marsyas lib - Please activate MARSYAS/MARSYAS_STATIC option")

endif (MARSYAS_STATIC)

## LINKING with MaxSDK libs
TARGET_LINK_LIBRARIES(ibt~ ${MAXSDK_API_LIBRARY} ${MAXSDK_CRT_LIBRARY} ${MAXSDK_AUDIO_LIBRARY})
# Define external extension as .mxe
SET_TARGET_PROPERTIES(ibt~ PROPERTIES SUFFIX ".mxe")

if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(ibt~ ${mad_LIBRARY})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(ibt~ ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(ibt~ ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (GSTREAMER_FOUND)
	TARGET_LINK_LIBRARIES(ibt~ ${GSTREAMER_LIBRARIES})
endif (GSTREAMER_FOUND)



