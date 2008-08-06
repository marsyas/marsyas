## setup OS_LIBS
include(marsyas-oslibs)


## global includes
include_directories(${CMAKE_SOURCE_DIR}/marsyasqt/)


## build apps
add_subdirectory(MarPlayer)
add_subdirectory(MarPhasevocoder)

