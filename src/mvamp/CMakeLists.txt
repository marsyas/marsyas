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

# maybe?
#include_directories(${CMAKE_SOURCE_DIR}/otherlibs/libsvm/)

add_library(mvamp SHARED ${MVamp_SOURCES} )

## LINKING
if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(marsyas ${mad_LIBRARY})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(marsyas ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(marsyas ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (GSTREAMER_FOUND)
	TARGET_LINK_LIBRARIES(marsyas ${GSTREAMER_LIBRARIES})
endif (GSTREAMER_FOUND)

TARGET_LINK_LIBRARIES(mvamp ${MVAMP_LIBRARIES})


