
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

if(MARSYAS_AUDIOIO AND NOT WITH_CPP11)
  message(FATAL_ERROR "MARSYAS_AUDIOIO is enabled, but it requires WITH_CPP11 which is disabled.")
endif()

if(WITH_CPP11)
  set(MARSYAS_REALTIME ON)
else()
  message(STATUS "'Realtime' features and dependent apps disabled, because WITH_CPP11 is disabled")
  set(MARSYAS_REALTIME OFF)
endif()

## detect packages
include (marsyas-detect)

## general setup

if(WITH_CPP11)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
  endif()
endif()

## CONFIG.H
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})
## oops, don't install this!  -gp
#install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/marsyas)

## setup includes
include (marsyas-includes)

## setup oslibs
include (marsyas-oslibs)




## next level
add_subdirectory(otherlibs)

add_subdirectory(marsyas)
if (MARSYAS_BUILD_APPS)
  add_subdirectory(apps)
endif (MARSYAS_BUILD_APPS)

if (SWIG_FOUND) 
	add_subdirectory(swig)
endif (SWIG_FOUND)

if(WITH_QT5)
  add_subdirectory(qt5apps)
endif()

if (QT4_FOUND)
  SET(QT_USE_QTNETWORK ON)
  SET(QT_USE_QTXML ON)
  if(WITH_OPENGL)
    SET(QT_USE_QTOPENGL TRUE)
  endif()
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
	add_subdirectory (tests)
endif (MARSYAS_TESTS)

## packaging
include(marsyas-package)


