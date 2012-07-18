
if (MARSYAS_WIN32)
	
	find_path(PD_MAIN_PATH /pd/include/)
	
	if (PD_MAIN_PATH)
		find_path(PD_INCLUDE_DIR m_pd.h HINTS "${PD_MAIN_PATH}/include/")
		#pd.lib is needed in Win32
		find_library(PD_LIB NAMES pd.lib HINTS "${PD_MAIN_PATH}/bin/")
	endif (PD_MAIN_PATH)
	
	if (PD_INCLUDE_DIR AND PD_LIB)
		set(PD_FOUND TRUE)
	endif (PD_INCLUDE_DIR AND PD_LIB)
	
else (MARSYAS_WIN32)

	if (MARSYAS_LINUX)
		find_path(PD_INCLUDE_DIR m_pd.h HINTS "/usr/include/pdextended")
	elseif (MARSYAS_MACOSX)
		find_path(PD_INCLUDE_DIR m_pd.h HINTS "/Applications/Pd-extended.app/Contents/Resources/include")
	endif (MARSYAS_LINUX)
	
	if (PD_INCLUDE_DIR)
		set(PD_FOUND TRUE)
	endif (PD_INCLUDE_DIR)

endif (MARSYAS_WIN32)

if (PD_FOUND)
	if (NOT PD_FIND_QUIETLY)
		message (STATUS "Found PD: ${PD_INCLUDE_DIR}")
		if (MARSYAS_WIN32)
			message (STATUS "Found PD lib: ${PD_LIB}")
		endif (MARSYAS_WIN32)
	endif (NOT PD_FIND_QUIETLY)
else (PD_FOUND)
	if (PD_FIND_REQUIRED)
		message (FATAL_ERROR "Could not find PD")
	endif (PD_FIND_REQUIRED)
endif (PD_FOUND)


