
file(GLOB Marsyas_PYTHON_MODULES
  RELATIVE ${CMAKE_SOURCE_DIR}/src/swig/python/modules *.py)

install(
  FILES ${Marsyas_PYTHON_MODULES}
  DESTINATION ${PYTHON_MODULES_DIR}
  )


