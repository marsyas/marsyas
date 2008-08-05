ADD_EXECUTABLE(sfplay sfplay.cpp)
TARGET_LINK_LIBRARIES(sfplay marsyas ${OS_LIBS})

install(TARGETS sfplay DESTINATION bin)

