include_directories(${CMAKE_SOURCE_DIR}/marsyas/core/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/marsystems/)
include_directories(${CMAKE_SOURCE_DIR}/marsyas/optional/)

add_subdirectory(unit_tests)
add_subdirectory(black_box)

