cmake_minimum_required(VERSION 2.8.9)

find_package(Qt5Quick 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)

include_directories( ${CMAKE_SOURCE_DIR}/src/ )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/ )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/core )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyasqt/ )

add_subdirectory(common)

if(WITH_CPP11)
  add_subdirectory(inspector)
endif()

if(MARSYAS_AUDIOIO AND MARSYAS_REALTIME)
  add_subdirectory(MarPlayer)
  add_subdirectory(MarPhasevocoder)
endif()
