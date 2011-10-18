
if (WIN32)
	find_path(DSOUND_INCLUDE_DIR dsound.h
		PATHS	
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK (August 2008)/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK (June 2010)/Include"
		DOC "The directory where dsound.h resides"
	)
	mark_as_advanced(DSOUND_INCLUDE_DIR)

	find_library(DSOUND_LIBRARY dsound
		PATHS	
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK (August 2008)/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK (June 2010)/Lib/x86"
		DOC "The directory where the dsound library resides"
	)
	mark_as_advanced(DSOUND_LIBRARY)


	if (DSOUND_INCLUDE_DIR AND DSOUND_LIBRARY)
		set(DSOUND_FOUND TRUE)
	endif (DSOUND_INCLUDE_DIR AND DSOUND_LIBRARY)

	if (DSOUND_FOUND)
		if (NOT DSOUND_FIND_QUIETLY)
			message (STATUS "Found dsound: ${DSOUND_LIBRARY}")
		endif (NOT DSOUND_FIND_QUIETLY)
	else (DSOUND_FOUND)
		if (DSOUND_FIND_REQUIRED)
			message (FATAL_ERROR "Could find: dsound")
		endif (DSOUND_FIND_REQUIRED)
	endif (DSOUND_FOUND)

endif(WIN32)


