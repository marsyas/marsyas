## BASIC SETUP
include(marsyas-oslibs)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/otherlibs/)


## BUILD MACRO
macro (marsyas_unit_test UNIT_TEST UNIT_TEST_SOURCES)
add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES})
target_link_libraries(${UNIT_TEST} marsyas ${OS_LIBS})
add_test(${UNIT_TEST} ${UNIT_TEST})
endmacro (marsyas_unit_test)


## ACTUAL UNIT TESTS
marsyas_unit_test(NormMaxMin NormMaxMin_runner.cpp)
marsyas_unit_test(WekaSource WekaSource_runner.cpp)

# Copy the files needed to run the tests
configure_file(files/tiny.arff ${CMAKE_CURRENT_BINARY_DIR}/files/tiny.arff COPYONLY)
configure_file(files/normal.arff ${CMAKE_CURRENT_BINARY_DIR}/files/normal.arff COPYONLY)
configure_file(files/two-classes-one-attribute.arff ${CMAKE_CURRENT_BINARY_DIR}/files/two-classes-one-attribute.arff COPYONLY)
