## BASIC SETUP
set(TEST_INPUT ${CMAKE_SOURCE_DIR}/input/)
set(TEST_OUTPUT ${CMAKE_SOURCE_DIR}/output/)


## TESTING MACRO
macro( regression_audio REG_NAME REG_INPUT REG_OUTPUT REG_COMMAND REG_ARGS)
# otherwise the command-line args get all screwed up.
separate_arguments(REG_ARGS)
#  Can't get this to work.  :(
#add_custom_command( OUTPUT ${REG_NAME}.au
#	COMMAND "${REG_COMMAND} ${REG_INPUT}"
#)
add_test( ${REG_NAME}1 ${REG_COMMAND} ${REG_ARGS} ${REG_INPUT})
add_test( ${REG_NAME}2 ${EXECUTABLE_OUTPUT_PATH}/audioCompare
	${REG_OUTPUT} ${REG_NAME}.au )
endmacro(regression_audio)


## actual tests
set( NEXT_COMMAND
	-ws 100 -s 0.25 -l 0.1 -g 0.7 -f sfplay.au
)
regression_audio (sfplay right.wav right-sfplay.au
${EXECUTABLE_OUTPUT_PATH}/sfplay
"${NEXT_COMMAND}"
)




