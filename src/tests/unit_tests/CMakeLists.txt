include(marsyas-oslibs)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_DIR})

include_directories(${CMAKE_SOURCE_DIR}/otherlibs/)



set(UNIT_TEST
	TestNormMaxMin
)

#add_custom_command(
#	OUTPUT ${CMAKE_BINARY_DIR}/${UNIT_TEST}.cpp
#	COMMAND ${CMAKE_SOURCE_DIR}/otherlibs/cxxtest/cxxtestgen.py
#	ARGS ${UNIT_TEST}.h
#)

#add_custom_target(ALL
#add_custom_command
#	DEPENDS ${CMAKE_BINARY_DIR}/${UNIT_TEST}.cpp
#)

#add_executable(${UNIT_TEST} ${CMAKE_BINARY_DIR}/${UNIT_TEST}.cpp)
add_executable(runner NormMaxMin_runner.cpp)

target_link_libraries(runner marsyas ${OS_LIBS})


