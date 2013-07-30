include_directories(${CMAKE_SOURCE_DIR}/src/)
add_executable(marsyas-latency-test main.cpp)
target_link_libraries(marsyas-latency-test marsyas)
