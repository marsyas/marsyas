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
	plugins.cpp
)

add_library(mvamp SHARED ${MVamp_SOURCES})

include_directories(${VAMP_INCLUDE_DIR})

## LINKING
TARGET_LINK_LIBRARIES(mvamp marsyas ${VAMP_LIBRARY})

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



