set (MVamp_SOURCES
	MarsyasBExtractCentroid.cpp
	MarsyasBExtractLPCC.cpp
	MarsyasBExtractLSP.cpp
	MarsyasBExtractMFCC.cpp
	MarsyasBExtractRolloff.cpp
	MarsyasBExtractSCF.cpp
	MarsyasBExtractSFM.cpp
	MarsyasBExtractZeroCrossings.cpp
	ZeroCrossing.cpp
	MarsyasIBT.cpp
	plugins.cpp
)

add_library(mvamp SHARED ${MVamp_SOURCES})

include_directories(${VAMP_INCLUDE_DIR})

# In case we build the static version of Marsyas, we need to link
# to that specific archive library. Also linking some additional libs/frameworks
if (MARSYAS_STATIC)

	if(MARSYAS_MACOSX)
		set(MARSYAS_LIBRARY ${LIBRARY_OUTPUT_PATH}/libmarsyas.a)
		find_library(COREAUDIO CoreAudio "/System/Library/Frameworks")
		find_library(CARBON Carbon "/System/Library/Frameworks")
		TARGET_LINK_LIBRARIES(mvamp ${COREAUDIO} ${CARBON})
	elseif(MARSYAS_LINUX)
	
	# TODO
	
	elseif(MARSYAS_WIN32)
	
	# TODO
		
	endif(MARSYAS_MACOSX)
	
else (MARSYAS_STATIC)
	# Cmake knows the location of the libraries it builds
	# the search path to the dynamic lib will automatically be added
	# maryas will result in  a "-lmarsyas" linker option.
	set(MARSYAS_LIBRARY marsyas)
endif (MARSYAS_STATIC)

## LINKING
TARGET_LINK_LIBRARIES(mvamp ${MARSYAS_LIBRARY} ${VAMP_LIBRARY})


#unfortuned Windows trial for adding /EXPORT:vampGetPluginDescriptor to Visual Studio project (as referred in http://vamp-plugins.org/wiki/mtp2)
#install(TARGETS mvamp EXPORT vampGetPluginDescriptor DESTINATION ${LIBRARY_OUTPUT_PATH})

if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(mvamp ${mad_LIBRARY})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(mvamp ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(mvamp ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (GSTREAMER_FOUND)
	TARGET_LINK_LIBRARIES(mvamp ${GSTREAMER_LIBRARIES})
endif (GSTREAMER_FOUND)



