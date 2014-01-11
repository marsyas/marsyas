
execute_process(
  COMMAND ${PEAKCLUSTERING_EXECUTABLE}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/input/nearhou.wav
  -o ${PEAKCLUSTERING_OUTPUT_DIR}
  WORKING_DIRECTORY ${PEAKCLUSTERING_OUTPUT_DIR}
)

if(UNIX)
  execute_process(
    COMMAND diff
    ${CMAKE_CURRENT_SOURCE_DIR}/output/nearhou.peak
    ${PEAKCLUSTERING_OUTPUT_DIR}/nearhou.peak
    RESULT_VARIABLE comparison_error
  )
elseif(WIN32)
  execute_process(
    COMMAND fc
    ${CMAKE_CURRENT_SOURCE_DIR}/output/nearhou.peak
    ${PEAKCLUSTERING_OUTPUT_DIR}/nearhou.peak
    RESULT_VARIABLE comparison_error)
endif()

if(comparison_error)
  message(FATAL_ERROR "Produced peak file differs from reference")
endif()
