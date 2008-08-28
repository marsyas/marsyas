project(helloworld)
cmake_minimum_required(VERSION 2.6)

## set as appropriate
set(MARSYAS_INCLUDE_DIR ../../src/marsyas/ ../../build/)
#set(MARSYAS_INCLUDE_DIR ../../src/marsyas/)
#set(MARSYAS_INCLUDE_DIR /usr/local/include/)

## set as appropriate
find_library(MARSYAS_LIBRARY marsyas
	PATHS
		../../build/lib/
		/usr/local/lib/
)

include_directories(${MARSYAS_INCLUDE_DIR})
add_executable(helloworld helloworld.cpp)
target_link_libraries(helloworld ${MARSYAS_LIBRARY})
#${MARSYAS_LIBRARY})

