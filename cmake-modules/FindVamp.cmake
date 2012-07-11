
find_path(VAMP_MAINPATH vamp-sdk/Plugin.h DOC "Insert main path to VampSDK (download it from http://www.vamp-plugins.org/develop.html for all platforms)")

if (VAMP_MAINPATH)
	find_path(VAMP_INCLUDE_DIR vamp-sdk/Plugin.h HINTS ${VAMP_MAINPATH})
	find_library(VAMP_LIBRARY NAMES VampPluginSDK HINTS ${VAMP_MAINPATH}/build/Release)
endif (VAMP_MAINPATH)

if (VAMP_INCLUDE_DIR AND VAMP_LIBRARY)
	set(VAMP_FOUND TRUE)
endif (VAMP_INCLUDE_DIR AND VAMP_LIBRARY)

if (VAMP_FOUND)
	if (NOT VAMP_FIND_QUIETLY)
		message (STATUS "Found VAMP: ${VAMP_LIBRARY}")
	endif (NOT VAMP_FIND_QUIETLY)
else (VAMP_FOUND)
	if (VAMP_FIND_REQUIRED)
		message (FATAL_ERROR "Could not find VAMP")
	endif (VAMP_FIND_REQUIRED)
endif (VAMP_FOUND)


