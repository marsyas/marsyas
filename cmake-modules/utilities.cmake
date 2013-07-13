
macro(target_enable_cpp11 TARGET_NAME)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set_property(TARGET ${TARGET_NAME} APPEND PROPERTY COMPILE_FLAGS "-std=c++11")
  elseif(CMAKE_COMPILER_IS_CLANG)
    set_property(TARGET ${TARGET_NAME} APPEND PROPERTY COMPILE_FLAGS "-std=c++11")
  endif()
endmacro()
