set (ibtpd_SOURCES
	ibt_pd.cpp
	IBT.cpp
)

add_library(ibt_pd SHARED ${ibtpd_SOURCES})

#include for all platforms
include_directories(${PD_INCLUDE_DIR})

# In case we build the static version of Marsyas, we need to link
# to that specific archive library. Also linking some additional libs/frameworks
if (MARSYAS_STATIC)

	if(MARSYAS_MACOSX)
		set(MARSYAS_LIBRARY ${LIBRARY_OUTPUT_PATH}/libmarsyas.a)
		find_library(COREAUDIO CoreAudio "/System/Library/Frameworks")
		find_library(CARBON Carbon "/System/Library/Frameworks")
		TARGET_LINK_LIBRARIES(ibt_pd ${COREAUDIO} ${CARBON})
	elseif(MARSYAS_LINUX)
		set(MARSYAS_LIBRARY ${LIBRARY_OUTPUT_PATH}/libmarsyas.a)
	elseif(MARSYAS_WIN32)
		set(MARSYAS_LIBRARY marsyas)
	endif(MARSYAS_MACOSX)
	
else (MARSYAS_STATIC)
	# Cmake knows the location of the libraries it builds
	# the search path to the dynamic lib will automatically be added
	# maryas will result in  a "-lmarsyas" linker option.
	set(MARSYAS_LIBRARY marsyas)
endif (MARSYAS_STATIC)

## LINKING AND DEFINITIONS
if(MARSYAS_MACOSX)
	#define -undefined dynamic_lookup
	TARGET_LINK_LIBRARIES(ibt_pd ${MARSYAS_LIBRARY} "-undefined dynamic_lookup")
	#define dylib extension as .pd_darwin in MacOSX 
	SET_TARGET_PROPERTIES(ibt_pd PROPERTIES SUFFIX ".pd_darwin")
elseif(MARSYAS_LINUX)
	#define --export-dynamic -shared
	TARGET_LINK_LIBRARIES(ibt_pd ${MARSYAS_LIBRARY} "--export-dynamic -shared")
	#define so extension as .pd_linux in Linux 
	SET_TARGET_PROPERTIES(ibt_pd PROPERTIES SUFFIX ".pd_linux")
elseif(MARSYAS_WIN32)	
	#also link pd.lib
	TARGET_LINK_LIBRARIES(ibt_pd ${MARSYAS_LIBRARY} ${PD_LIB})
	#active Debug info in Release mode - for some reason when compiling with MSVC this is needed
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /debug")
	#add /export:ibt_pd_setup to export setup function from the dll
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /export:ibt_pd_setup")
	#Add MSW preprocessor definition
	add_definitions(-DMSW)
endif(MARSYAS_MACOSX)

#no lib prefix
SET_TARGET_PROPERTIES(ibt_pd PROPERTIES PREFIX "")

if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(ibt_pd ${mad_LIBRARY})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(ibt_pd ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(ibt_pd ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (GSTREAMER_FOUND)
	TARGET_LINK_LIBRARIES(ibt_pd ${GSTREAMER_LIBRARIES})
endif (GSTREAMER_FOUND)



