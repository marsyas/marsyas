#Can't define target with ~ in MacOSX -> Best alternative "ibt~ "#
set(ibt_ext "ibt_")

project(${ibt_ext})

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

# (for now) only works on the static version of Marsyas
# we need to link to that specific archive library. 
# Also linking some additional libs/frameworks
if (MARSYAS_STATIC)

	# TESTED ON MAC/OSX (10.5.8) -> FOR XCODE ONLY!!! (tested on Xcode3.0) -> FOR i386arch (32bits) ONLY (Max/MSP only runs at 32bits, despite the OS architecture)
	if(MARSYAS_MACOSX)

		add_executable(${ibt_ext} MACOSX_BUNDLE ${MarMax_SOURCES} ${MarMax_HEADERS})

		include_directories(${MAXSDK_MAX_INCLUDE_DIR})
		include_directories(${MAXSDK_MSP_INCLUDE_DIR})
		#include_directories(/Developer/Headers/FlatCarbon)

		#link static Marsyas lib
		TARGET_LINK_LIBRARIES(${ibt_ext} "${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/libmarsyas.a")
		
		#link audio/midi and XCode carbon frameworks
		FIND_LIBRARY(CARBON_LIBRARY Carbon)
		FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
		FIND_LIBRARY(COREMIDI_LIBRARY CoreMidi)
		TARGET_LINK_LIBRARIES(${ibt_ext} ${CARBON_LIBRARY} ${COREAUDIO_LIBRARY} ${COREMIDI_LIBRARY})
		
		#Pre-processor defines
		ADD_DEFINITIONS(-DMAC_VERSION)

		## LINKING with MaxSDK libs
		TARGET_LINK_LIBRARIES(${ibt_ext} ${MAXSDK_API_LIBRARY} ${MAXSDK_AUDIO_LIBRARY})

		# Max/MSP External specifications for XCode ONLY
		# Define Mach-O Type to Bundle
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_MACH_O_TYPE "mh_bundle")
		# Define external extension as .mxo
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_WRAPPER_EXTENSION "mxo")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${MAXSDK_MAINPATH}/c74support/max-includes/macho-prefix.pch")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE "${MAXSDK_MAINPATH}/examples/Info.plist")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_WARNING_CFLAGS "-Wmost -Wno-four-char-constants -Wno-unknown-pragmas")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_DEPLOYMENT_LOCATION "YES")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE "YES")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_DSTROOT "${EXECUTABLE_OUTPUT_PATH}")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "/${CMAKE_BUILD_TYPE}")
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES XCODE_ATTRIBUTE_GCC_DYNAMIC_NO_PIC "NO")

	# TESTED ON WINDOWS (Windows7_64bits) -> FOR i386arch (32bits) ONLY (Max/MSP only runs at 32bits, despite the OS architecture)
	elseif(MARSYAS_WIN32)
	
		add_library(${ibt_ext} SHARED ${MarMax_SOURCES} ${MarMax_HEADERS})

		include_directories(${MAXSDK_MAX_INCLUDE_DIR})
		include_directories(${MAXSDK_MSP_INCLUDE_DIR})

		TARGET_LINK_LIBRARIES(${ibt_ext} marsyas)
		#Pre-processor defines
		ADD_DEFINITIONS(-DWIN_VERSION -DEXT_WIN_VERSION)
		# Define external extension as .mxe
		SET_TARGET_PROPERTIES(${ibt_ext} PROPERTIES SUFFIX ".mxe")

		## LINKING with MaxSDK libs
		TARGET_LINK_LIBRARIES(${ibt_ext} ${MAXSDK_API_LIBRARY} ${MAXSDK_CRT_LIBRARY} ${MAXSDK_AUDIO_LIBRARY})

	endif(MARSYAS_MACOSX)

#display warning if Marsyas not in STATIC mode (only works if statically linked to Marsyas lib)
else (MARSYAS_STATIC)
	 
	 message(SEND_ERROR "Only working linked statically to Marsyas lib - Please activate MARSYAS/MARSYAS_STATIC option")

endif (MARSYAS_STATIC)


if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(${ibt_ext} ${mad_LIBRARY})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(${ibt_ext} ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(${ibt_ext} ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (GSTREAMER_FOUND)
	TARGET_LINK_LIBRARIES(${ibt_ext} ${GSTREAMER_LIBRARIES})
endif (GSTREAMER_FOUND)


