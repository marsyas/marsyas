enable_testing()

set(TEST_SOURCE ${CMAKE_SOURCE_DIR}/../regressionTests/input/)
set(TEST_ANSWER ${CMAKE_SOURCE_DIR}/../regressionTests/answers/)
set(TEST_OUT ${CMAKE_BINARY_DIR}/test-output)
make_directory(${TEST_OUT})


add_test(sfplay1
	${EXECUTABLE_OUTPUT_PATH}/sfplay
	-ws 100 -s 0.25 -l 0.1 -g 0.7 -f ${TEST_OUT}/out.au
	${TEST_SOURCE}/right.wav
)

add_test(sfplay2
	${EXECUTABLE_OUTPUT_PATH}/audioCompare
	${TEST_OUT}/out.au
	${TEST_ANSWER}/right-sfplay.au
)


