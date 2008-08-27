
if (WIN32)
	find_path(dsound_INCLUDE_DIR dsound.h
		PATHS	
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where dsound.h resides"
	)
	mark_as_advanced(dsound_INCLUDE_DIR)

# maybe?
#	find_library(dsound_LIBRARY NAMES dsound
	find_library(dsound_LIBRARY dsound
		PATHS	
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib"
		DOC "The directory where dsound.h resides"
	)
	mark_as_advanced(dsound_LIBRARY)



	if (dsound_INCLUDE_DIR AND dsound_LIBRARY)
		set(dsound_FOUND TRUE)
	endif (dsound_INCLUDE_DIR AND dsound_LIBRARY)

	if (dsound_FOUND)
		if (NOT dsound_FIND_QUIETLY)
			message (STATUS "Found dsound: ${dsound_LIBRARY}")
		endif (NOT dsound_FIND_QUIETLY)
	else (dsound_FOUND)
		if (dsound_FIND_REQUIRED)
			message (FATAL_ERROR "Could find: dsound")
		endif (dsound_FIND_REQUIRED)
	endif (dsound_FOUND)

endif(WIN32)


