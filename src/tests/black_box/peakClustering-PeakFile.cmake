set(CMAKE_CRT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})


execute_process(COMMAND peakClustering -P ${CMAKE_CURRENT_SOURCE_DIR}/input/nearhou.wav -o ${PEAKCLUSTERING_OUTPUT_DIR}
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(UNIX)				
execute_process(COMMAND diff ${CMAKE_CRT_SRC_DIR}/output/nearhou.peak ${PEAKCLUSTERING_OUTPUT_DIR}/nearhou.peak
				RESULT_VARIABLE rv)
elseif (WIN32)
execute_process(COMMAND fc ${CMAKE_CRT_SRC_DIR}/output/nearhou.peak ${PEAKCLUSTERING_OUTPUT_DIR}/nearhou.peak
				RESULT_VARIABLE rv)
endif()
				
if(rv)
	message(FATAL_ERROR "Peak file differs from reference") 
endif()