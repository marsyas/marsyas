find_path(mad_INCLUDE_DIR mad.h)
find_library(mad_LIBRARY NAMES mad)

if (mad_INCLUDE_DIR AND mad_LIBRARY)
	set(mad_FOUND TRUE)
endif (mad_INCLUDE_DIR AND mad_LIBRARY)

if (mad_FOUND)
	if (NOT mad_FIND_QUIETLY)
		message (STATUS "Found mad: ${mad_LIBRARY}")
	endif (NOT mad_FIND_QUIETLY)
else (mad_FOUND)
	if (mad_FIND_REQUIRED)
		message (FATAL_ERROR "Could find: mad")
	endif (mad_FIND_REQUIRED)
endif (mad_FOUND)


