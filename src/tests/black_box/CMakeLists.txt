## BASIC SETUP
set(TEST_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/input)
set(TEST_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(TEST_TEMPDIR ${CMAKE_CURRENT_BINARY_DIR})
set(MARSYAS_SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/scripts/)

find_package(PythonInterp REQUIRED)

if (MARSYAS_MACOSX)
  set(COMMAND_LINE_LIBRARY_ENV DYLD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/lib )
else (MARSYAS_MACOSX)
  set(COMMAND_LINE_LIBRARY_ENV "")
endif (MARSYAS_MACOSX)

## TESTING MACRO
macro( black_box_audio REG_NAME REG_INPUT REG_OUTPUT REG_COMMAND REG_ARGS)
# otherwise the command-line args get all screwed up.
separate_arguments(REG_ARGS)
#  Can't get this to work.  :(
#add_custom_command( OUTPUT ${REG_NAME}.au
#	COMMAND "${REG_COMMAND} ${REG_INPUT}"
#)
add_test(
  NAME ${REG_NAME}-creation
  COMMAND $<TARGET_FILE:${REG_COMMAND}>
    ${REG_ARGS}
    "${TEST_INPUT}/${REG_INPUT}"
)
add_test(
  NAME ${REG_NAME}-verification
  COMMAND $<TARGET_FILE:audioCompare>
    "${TEST_OUTPUT}/${REG_OUTPUT}"
    "${REG_OUTPUT}"
)
endmacro(black_box_audio)


## actual tests
set( NEXT_COMMAND
	-ws 100 -sa 0.25 -ln 0.1 -g 0.7 -o right-sfplay.au
)
black_box_audio( sfplay right.wav right-sfplay.au sfplay "${NEXT_COMMAND}")


set( NEXT_COMMAND
	-q -p 1.5 -s 50 -g 0.9 -ob -f right-phasevocoder.au
)
black_box_audio (phasevocoder right.wav right-phasevocoder.au
	phasevocoder "${NEXT_COMMAND}")


set( NEXT_COMMAND
)
black_box_audio (peakClustering nearhou.wav nearhouSep.wav
	peakClustering "${NEXT_COMMAND}")

add_test(NAME peakClustering-peaks
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND cmake
  -DPEAKCLUSTERING_EXECUTABLE=$<TARGET_FILE:peakClustering>
  -DPEAKCLUSTERING_OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/test-peakClustering-peaks.cmake
)

macro(bextract_test REG_NAME DIR MF_INPUT EXTRA_BEXTRACT)
  add_test(
    NAME ${REG_NAME}-bextract
    COMMAND "${PythonInterp_EXECUTABLE}"
      ${MARSYAS_SCRIPTS_DIR}/cmake-test-wrapper.py
      ${DIR}
      ${COMMAND_LINE_LIBRARY_ENV}
      $<TARGET_FILE:bextract> -fe
      ${EXTRA_BEXTRACT}
      -w ${REG_NAME}.arff
      -od ${TEST_TEMPDIR}/
      ${MF_INPUT}
  )
endmacro(bextract_test)

macro(bextract_kea_test REG_NAME DIR MF_INPUT EXTRA_BEXTRACT)
  bextract_test(${REG_NAME} ${DIR} ${MF_INPUT} ${EXTRA_BEXTRACT} )
  # we don't need the TEST_TEMPDIR for the kea commands
  add_test(
    NAME ${REG_NAME}-classify-gs
    COMMAND "${PythonInterp_EXECUTABLE}"
      ${MARSYAS_SCRIPTS_DIR}/kea-wrapper.py
      $<TARGET_FILE:kea>
      -w ${TEST_TEMPDIR}/${REG_NAME}.arff -cl GS
      ${REG_NAME}-gs.txt
      ${TEST_OUTPUT}/${REG_NAME}-gs.txt
  )
  add_test(
    NAME ${REG_NAME}-classify-svm
    COMMAND "${PythonInterp_EXECUTABLE}"
      ${MARSYAS_SCRIPTS_DIR}/kea-wrapper.py
      $<TARGET_FILE:kea>
      -w ${TEST_TEMPDIR}/${REG_NAME}.arff -cl SVM
      ${REG_NAME}-svm.txt
      ${TEST_OUTPUT}/${REG_NAME}-svm.txt
  )
endmacro(bextract_kea_test)

macro(tempo_test REG_NAME DIR MF_INPUT)
  add_test(
    NAME ${REG_NAME}-tempo
    COMMAND "${PythonInterp_EXECUTABLE}"
      ${MARSYAS_SCRIPTS_DIR}/tempo-wrapper.py
      $<TARGET_FILE:tempo>
      ${DIR}/${MF_INPUT}
      ${TEST_TEMPDIR}/${REG_NAME}-tempo.txt
      ${TEST_OUTPUT}/${REG_NAME}-tempo.txt
  )
endmacro(tempo_test)

macro(timeline_test REG_NAME DIR MF_INPUT EXTRA_BEXTRACT)
  add_test(
    NAME ${REG_NAME}-bextract
    COMMAND "${PythonInterp_EXECUTABLE}"
      ${MARSYAS_SCRIPTS_DIR}/timeline-test-wrapper.py
      ${DIR}
      ${COMMAND_LINE_LIBRARY_ENV}
      $<TARGET_FILE:bextract> -fe
      ${EXTRA_BEXTRACT}
      -w ${TEST_TEMPDIR}/${REG_NAME}.arff
      -od ${TEST_TEMPDIR}/
      ${MF_INPUT}
      ${TEST_OUTPUT}/${REG_NAME}.arff
  )
endmacro(timeline_test)

macro(sfplugin_test REG_NAME DIR MF_INPUT EXTRA_BEXTRACT TEST_FILE)
  add_test(
    NAME ${REG_NAME}-bextract
    COMMAND "${PythonInterp_EXECUTABLE}"
      ${MARSYAS_SCRIPTS_DIR}/sfplugin-test-wrapper.py
      ${DIR}
      $<TARGET_FILE:bextract>
      "$<TARGET_FILE:sfplugin> -pm"
      "${EXTRA_BEXTRACT}"
      ${MF_INPUT}
      ${TEST_TEMPDIR}/${REG_NAME}.mpl
      ${TEST_INPUT}/${TEST_FILE}
      ${TEST_OUTPUT}/${REG_NAME}.txt
  )
endmacro(sfplugin_test)


### bextract + kea tests
bextract_kea_test( basic-waves
  ${TEST_INPUT}/basic-waves basic-waves.mf
  "-m 0"
)
bextract_kea_test( basic-waves-os
  ${TEST_INPUT}/basic-waves basic-waves.mf
  "-m 0 -os"
)
bextract_test( basic-waves-regression
  ${TEST_INPUT}/basic-waves basic-waves.mf
  "-m 0 -rg"
)

timeline_test( timeline-simple
  ${TEST_INPUT} timeline-sine2048-square2048.mf
  "-m 0 -rlf -t"
)

sfplugin_test( sfplugin-simple
  ${TEST_INPUT} timeline-sine2048-square2048.mf
  "-od ${TEST_TEMPDIR}/ -ws 128 -hp 128 -pm"
  bd22k.wav
)

sfplugin_test( sfplugin-csv
  ${TEST_INPUT} timeline-sine2048-square2048.mf
  "-od ${TEST_TEMPDIR}/ -ws 128 -hp 128 -pm -csv"
  bd22k.wav
)


if (EXISTS ${TEST_MINI_GENRES_DIR})
  bextract_kea_test( mini-genres
    ${TEST_MINI_GENRES_DIR}/mini-genres/ mini-genres.mf
    "-ds 50"
  )

  tempo_test( mini-genres
    ${TEST_MINI_GENRES_DIR}/mini-genres/ mini-genres-tempos.mf
  )
endif (EXISTS ${TEST_MINI_GENRES_DIR})
