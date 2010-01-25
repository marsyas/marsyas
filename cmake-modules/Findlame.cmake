find_path(lame_INCLUDE_DIR lame/lame.h PATHS /opt/local/include NO_DEFAULT_PATH)
find_path(lame_INCLUDE_DIR lame/lame.h)

find_library(lame_LIBRARY NAMES mp3lame PATHS /opt/local/lib NO_DEFAULT_PATH)
find_library(lame_LIBRARY NAMES mp3lame)

if (lame_INCLUDE_DIR AND lame_LIBRARY)
	set(lame_FOUND TRUE)
endif (lame_INCLUDE_DIR AND lame_LIBRARY)

if (lame_FOUND)
	if (NOT lame_FIND_QUIETLY)
		message (STATUS "Found lame: ${lame_LIBRARY}")
	endif (NOT lame_FIND_QUIETLY)
else (lame_FOUND)
	if (lame_FIND_REQUIRED)
		message (FATAL_ERROR "Could not find: lame")
	endif (lame_FIND_REQUIRED)
endif (lame_FOUND)


