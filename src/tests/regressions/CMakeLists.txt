## BASIC SETUP
set(TEST_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/input)
set(TEST_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(TEST_TEMPDIR ${CMAKE_CURRENT_BINARY_DIR})

if (MARSYAS_MACOSX)
  set(COMMAND_LINE_LIBRARY_ENV DYLD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/lib )
endif (MARSYAS_MACOSX)

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
	${TEST_OUTPUT}/${REG_OUTPUT} ${REG_OUTPUT} )
endmacro(regression_audio)


## actual tests
set( NEXT_COMMAND
	-ws 100 -s 0.25 -l 0.1 -g 0.7 -f right-sfplay.au
)
regression_audio( sfplay right.wav right-sfplay.au sfplay "${NEXT_COMMAND}")


set( NEXT_COMMAND
	-q -p 1.5 -s 50 -g 0.9 -ob -f right-phasevocoder.au
)
regression_audio (phasevocoder right.wav right-phasevocoder.au
	phasevocoder "${NEXT_COMMAND}")


set( NEXT_COMMAND
)
regression_audio (peakClustering nearhou.wav nearhouSep.wav
	peakClustering "${NEXT_COMMAND}")


# copied from ../unit_tests/CMakeLists.txt
if (MARSYAS_STATIC)
	# Cmake knows the location of the libraries it builds
	# the search path to the dynamic lib will automatically be added
	# maryas will result in  a "-lmarsyas" linker option.
	set(MARSYAS_LIBRARIES marsyas)
endif (MARSYAS_STATIC)

# tiny bextract example
add_test( basic-waves-extract-generate
  ${CMAKE_SOURCE_DIR}/../scripts/cmake-test-wrapper.py
    ${TEST_INPUT}/basic-waves ${COMMAND_LINE_LIBRARY_ENV}
    ${EXECUTABLE_OUTPUT_PATH}/bextract -fe
    -m 0
    -w ${TEST_TEMPDIR}/basic-waves.arff
    ${TEST_INPUT}/basic-waves/basic-waves.mf
)


add_test( basic-waves-classify-gs
  ${CMAKE_SOURCE_DIR}/../scripts/kea-wrapper.py ${EXECUTABLE_OUTPUT_PATH}/kea -w basic-waves.arff -cl GS basic-waves-gs.txt ${TEST_OUTPUT}/basic-waves-gs.txt
)
add_test( basic-waves-classify-svm
  ${CMAKE_SOURCE_DIR}/../scripts/kea-wrapper.py ${EXECUTABLE_OUTPUT_PATH}/kea -w basic-waves.arff -cl SVM basic-waves-svm.txt ${TEST_OUTPUT}/basic-waves-svm.txt
)

if (EXISTS ${TEST_MINI_GENRES_DIR})
  add_test( genre-extract-generate
    ${CMAKE_SOURCE_DIR}/../scripts/cmake-test-wrapper.py
    ${TEST_MINI_GENRES_DIR} ${COMMAND_LINE_LIBRARY_ENV}
    ${EXECUTABLE_OUTPUT_PATH}/bextract -fe
    -ds 50
    -w ${TEST_TEMPDIR}/mini-genres.arff mini-genres.mf
  )
  add_test( genre-classify-gs
     ${CMAKE_SOURCE_DIR}/../scripts/kea-wrapper.py ${EXECUTABLE_OUTPUT_PATH}/kea -w mini-genres.arff -cl GS mini-genres-gs.txt ${TEST_OUTPUT}/mini-genres-gs.txt
  )
  add_test( genre-classify-svm
     ${CMAKE_SOURCE_DIR}/../scripts/kea-wrapper.py ${EXECUTABLE_OUTPUT_PATH}/kea -w mini-genres.arff -cl SVM mini-genres-svm.txt ${TEST_OUTPUT}/mini-genres-svm.txt
  )
endif (EXISTS ${TEST_MINI_GENRES_DIR})

