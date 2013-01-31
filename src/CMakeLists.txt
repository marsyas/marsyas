## global setup
cmake_minimum_required (VERSION 2.6)
project (marsyas)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake-modules")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Marsyas: Music Analysis, Retrieval and Synthesis for Audio Signals")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "4") 
SET(CPACK_PACKAGE_VERSION_PATCH "7") 
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_SOURCE_GENERATOR "TGZ")
INCLUDE(CPack)


## version
set (MARSYAS_VERSION_MAJOR "0")
set (MARSYAS_VERSION_MINOR "4")
set (MARSYAS_VERSION_PATCH "7")


## basic CMAKE vars
mark_as_advanced (EXECUTABLE_OUTPUT_PATH)
mark_as_advanced (LIBRARY_OUTPUT_PATH)
if (APPLE)
	mark_as_advanced (CMAKE_OSX_ARCHITECTURES)
	mark_as_advanced (CMAKE_OSX_SYSROOT)
endif (APPLE)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
if (MSVC)
## windows likes DLL near executables 
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
else (MSVC) 
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
endif (MSVC) 



## find OS
if(UNIX)
	if(APPLE)
		set(MARSYAS_MACOSX 1)
	elseif(CYGWIN)
		set(MARSYAS_CYGWIN 1)
	else(APPLE)
		set(MARSYAS_LINUX 1)
	endif(APPLE)
elseif(MINGW)
	set(MARSYAS_MINGW 1)
elseif(MSVC)
	set(MARSYAS_WIN32 1)
endif(UNIX)

# for testing
#MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: LINUX " ${MARSYAS_LINUX})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: OSX " ${MARSYAS_MACOSX})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: WIN32 " ${MARSYAS_WIN32})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: MINGW " ${MARSYAS_MINGW})
#MESSAGE( STATUS "MARSYAS_OPERATION_SYSTEM: CYGWIN " ${MARSYAS_CYGWIN})

include (TestBigEndian)
test_big_endian(MARSYAS_BIGENDIAN)


## user options
include (marsyas-options)

## detect packages
include (marsyas-detect)

## CONFIG.H
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})
## oops, don't install this!  -gp
#install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/marsyas)

## setup includes
include (marsyas-includes)

## setup oslibs
include (marsyas-oslibs)


## next level
add_subdirectory(marsyas)
if (MARSYAS_BUILD_APPS)
  add_subdirectory(apps)
endif (MARSYAS_BUILD_APPS)

if (MARSYAS_PNG)  
   add_subdirectory(otherlibs)
endif (MARSYAS_PNG)

if (SWIG_FOUND) 
	add_subdirectory(swig)
endif (SWIG_FOUND)

if (QT4_FOUND)
        SET(QT_USE_QTNETWORK ON)
	SET(QT_USE_QTXML ON)
	include (${QT_USE_FILE})
        add_subdirectory(marsyasqt)
        add_subdirectory(Qt4Apps)
endif (QT4_FOUND)

if (VAMP_FOUND)
	add_subdirectory(mvamp)
endif (VAMP_FOUND)

if (MAXSDK_FOUND)
	add_subdirectory(marsyas_max-msp)
endif (MAXSDK_FOUND)

if (PD_FOUND)
	add_subdirectory(marsyas_pd)
endif (PD_FOUND)

## tests, for verification
if (MARSYAS_TESTS)
	enable_testing()
  include(CTest)
	add_subdirectory (tests)
endif (MARSYAS_TESTS)

## packaging
include(marsyas-package)


