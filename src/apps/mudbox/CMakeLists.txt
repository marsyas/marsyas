ADD_EXECUTABLE(mudbox mudbox.cpp)
TARGET_LINK_LIBRARIES(mudbox marsyas ${OS_LIBS})
install(TARGETS mudbox DESTINATION bin) 

