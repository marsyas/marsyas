## setup OS_LIBS
include(marsyas-oslibs)


## build apps
add_subdirectory(sfplay)
add_subdirectory(bextract)
add_subdirectory(sfinfo)
add_subdirectory(phasevocoder)
add_subdirectory(mudbox)
add_subdirectory(sfplugin)
add_subdirectory(tempo) 
add_subdirectory(virtualsensor)

