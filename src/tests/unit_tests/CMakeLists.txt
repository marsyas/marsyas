## BASIC SETUP
include(marsyas-oslibs)
include_directories(${CMAKE_SOURCE_DIR}/otherlibs/)
include_directories(${LIBRARY_OUTPUT_PATH})

# In case we build the static version of Marsyas, we need to link
# to that specific archive library. Also linking some additional libs/frameworks
if (MARSYAS_STATIC)

	if(MARSYAS_MACOSX)
		find_library(COREAUDIO CoreAudio "/System/Library/Frameworks")
		find_library(CARBON Carbon "/System/Library/Frameworks")
		set(MARSYAS_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libmarsyas.a ${COREAUDIO} ${CARBON})
		
	elseif(MARSYAS_LINUX)
	
		# TODO
		set(MARSYAS_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libmarsyas.a)
	
	elseif(MARSYAS_WIN32)
	
		# TODO
		set(MARSYAS_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libmarsyas.lib)
		
	endif(MARSYAS_MACOSX)
	
else (MARSYAS_STATIC)
	# Cmake knows the location of the libraries it builds
	# the search path to the dynamic lib will automatically be added
	# maryas will result in  a "-lmarsyas" linker option.
	set(MARSYAS_LIBRARIES marsyas)
endif (MARSYAS_STATIC)



## BUILD MACRO
macro (marsyas_unit_test UNIT_TEST UNIT_TEST_SOURCES)
add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES})
target_link_libraries(${UNIT_TEST} ${MARSYAS_LIBRARIES} ${OS_LIBS})
add_test(${UNIT_TEST} ${EXECUTABLE_OUTPUT_PATH}/${UNIT_TEST})
endmacro (marsyas_unit_test)


## ACTUAL UNIT TESTS
marsyas_unit_test(NormMaxMin NormMaxMin_runner.cpp)
marsyas_unit_test(WekaSource WekaSource_runner.cpp)
marsyas_unit_test(Selector Selector_runner.cpp)
marsyas_unit_test(AubioYin AubioYin_runner.cpp)
marsyas_unit_test(RunningAutocorrelation RunningAutocorrelation_runner.cpp)
marsyas_unit_test(SubtractMean SubtractMean_runner.cpp)
marsyas_unit_test(RunningStatistics RunningStatistics_runner.cpp)
marsyas_unit_test(AimPZFC AimPZFC_runner.cpp)
marsyas_unit_test(AimGammatone AimGammatone_runner.cpp)
marsyas_unit_test(AimHCL AimHCL_runner.cpp)
marsyas_unit_test(AimLocalMax AimLocalMax_runner.cpp)
marsyas_unit_test(AimSAI AimSAI_runner.cpp)
marsyas_unit_test(AimBoxes AimBoxes_runner.cpp)
marsyas_unit_test(Sum Sum_runner.cpp)
marsyas_unit_test(DeltaFirstOrderRegression DeltaFirstOrderRegression_runner.cpp)
marsyas_unit_test(DelaySamples DelaySamples_runner.cpp)
marsyas_unit_test(CARFAC CARFAC_runner.cpp)
marsyas_unit_test(HarmonicStrength HarmonicStrength_runner.cpp)

# Copy the files needed to run the tests
configure_file(files/tiny.arff ${CMAKE_CURRENT_BINARY_DIR}/files/tiny.arff COPYONLY)
configure_file(files/normal.arff ${CMAKE_CURRENT_BINARY_DIR}/files/normal.arff COPYONLY)
configure_file(files/two-classes-one-attribute.arff ${CMAKE_CURRENT_BINARY_DIR}/files/two-classes-one-attribute.arff COPYONLY)
configure_file(files/test.wav ${CMAKE_CURRENT_BINARY_DIR}/files/test.wav COPYONLY)
configure_file(files/binaural.wav ${CMAKE_CURRENT_BINARY_DIR}/files/binaural.wav COPYONLY)
configure_file(files/harmonic-mix.wav ${CMAKE_CURRENT_BINARY_DIR}/files/harmonic-mix.wav COPYONLY)

