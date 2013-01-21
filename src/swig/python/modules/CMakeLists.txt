
file(GLOB Marsyas_PYTHON_MODULES
  RELATIVE ${CMAKE_SOURCE_DIR}/swig/python/modules *.py)

install(
  FILES ${Marsyas_PYTHON_MODULES}
  DESTINATION ${PYTHON_INSTDIR}
  )


