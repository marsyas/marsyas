## BASIC SETUP
set(TEST_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/input/)
set(TEST_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/)


## TESTING MACRO
macro( regression_audio REG_NAME REG_INPUT REG_OUTPUT REG_COMMAND REG_ARGS)
# otherwise the command-line args get all screwed up.
separate_arguments(REG_ARGS)
#  Can't get this to work.  :(
#add_custom_command( OUTPUT ${REG_NAME}.au
#	COMMAND "${REG_COMMAND} ${REG_INPUT}"
#)
add_test( ${REG_NAME}-creation ${EXECUTABLE_OUTPUT_PATH}/${REG_COMMAND}
	${REG_ARGS} ${TEST_INPUT}/${REG_INPUT} )
add_test( ${REG_NAME}-verification ${EXECUTABLE_OUTPUT_PATH}/audioCompare
	${TEST_OUTPUT}/${REG_OUTPUT} ${REG_NAME}.au )
endmacro(regression_audio)


## actual tests
set( NEXT_COMMAND
	-ws 100 -s 0.25 -l 0.1 -g 0.7 -f sfplay.au
)
regression_audio( sfplay right.wav right-sfplay.au sfplay "${NEXT_COMMAND}")


# current broken  :(
set( NEXT_COMMAND
	-q -p 1.5 -s 50 -g 0.9 -f phasevocoder.au
)
regression_audio (phasevocoder right.wav right-phasevocoder.au
	phasevocoder "${NEXT_COMMAND}")


