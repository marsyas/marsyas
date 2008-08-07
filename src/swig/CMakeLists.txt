# setup OS_LIBS
include (marsyas-oslibs)

# to allow "marsyas/MarSystemManager.h" to work.
include_directories(${CMAKE_SOURCE_DIR})

# global swig stuff
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})


## build apps
add_subdirectory(python)

