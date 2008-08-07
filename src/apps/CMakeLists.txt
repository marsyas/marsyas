## setup OS_LIBS
include(marsyas-oslibs)

## BUILD MACRO
macro (marsyas_app APP APP_SOURCES)
add_executable(${APP} ${APP_SOURCES})
target_link_libraries(${APP} marsyas ${OS_LIBS})
install(TARGETS ${APP} DESTINATION bin)
endmacro (marsyas_app)

## OS-specific
if (MARSYAS_MACOSX)
	if (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
		add_definitions(-D__MACOSX_CORE__)
	endif (MARSYAS_AUDIOIO OR MARSYAS_MIDIIO)
endif (MARSYAS_MACOSX)

if (MARSYAS_LINUX)
	if (MARSYAS_AUDIOIO)
    		if (OSS)
			add_definitions(-D__LINUX_OSS__)
		else (OSS)
			add_definitions(-D__LINUX_ALSA__)
    		endif (OSS)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		if (OSS)
			add_definitions(-D__LINUX_OSS__)
		else (OSS)
			add_definitions(-D__LINUX_ALSASEQ__)
    		endif (OSS)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_LINUX)

if (MARSYAS_CYGWIN)
	if (MARSYAS_AUDIOIO)
		add_definitions(-D__WINDOWS_DS__)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_CYGWIN)

if (MARSYAS_MINGW)
	if (MARSYAS_AUDIOIO)
		add_definitions(-D__WINDOWS_DS__)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_MINGW)
 
if (MARSYAS_WIN32)
	if (MARSYAS_AUDIOIO)
		add_definitions(-D__WINDOWS_DS__)
	endif (MARSYAS_AUDIOIO)
	if (MARSYAS_MIDIIO)
		add_definitions(-D__WINDOWS_MM__)
	endif (MARSYAS_MIDIIO)
endif (MARSYAS_WIN32)



## maybe
if (WIN32)
  list (APPEND MarSystem_SOURCES
    common.h
  )
endif (WIN32)




## build apps
add_subdirectory(sfplay)
add_subdirectory(bextract)
add_subdirectory(sfinfo)
add_subdirectory(phasevocoder)
add_subdirectory(mudbox)
add_subdirectory(sfplugin)
add_subdirectory(tempo) 
add_subdirectory(virtualsensor)

