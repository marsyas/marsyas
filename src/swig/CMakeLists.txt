# setup OS_LIBS
include (marsyas-oslibs)

# to allow "marsyas/MarSystemManager.h" to work.
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/marsyas/core/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/marsystems/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/optional/)

# global swig stuff
#find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})


## build apps
if (WITH_SWIG_PYTHON)
	add_subdirectory(python)
endif (WITH_SWIG_PYTHON)
if (WITH_SWIG_JAVA)
	add_subdirectory(ruby)
endif (WITH_SWIG_JAVA)
if (WITH_SWIG_LUA)
	add_subdirectory(lua)
endif (WITH_SWIG_LUA)
if (WITH_SWIG_RUBY)
	add_subdirectory(ruby)
endif (WITH_SWIG_RUBY)

