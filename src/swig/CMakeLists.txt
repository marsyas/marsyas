# setup OS_LIBS
include (marsyas-oslibs)

# to allow "marsyas/MarSystemManager.h" to work.
include_directories(${CMAKE_SOURCE_DIR})


## build apps
add_subdirectory(python)

