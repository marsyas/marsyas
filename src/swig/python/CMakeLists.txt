
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/swig/)
configure_file(marsyas.py ${CMAKE_CURRENT_BINARY_DIR}/marsyas.py COPYONLY)
configure_file(pvoc.py ${CMAKE_CURRENT_BINARY_DIR}/pvoc.py COPYONLY)
configure_file(test.py ${CMAKE_CURRENT_BINARY_DIR}/test.py COPYONLY)
configure_file(testPylab.py ${CMAKE_CURRENT_BINARY_DIR}/testPylab.py
COPYONLY)
configure_file(chuck_like.py ${CMAKE_CURRENT_BINARY_DIR}/chuck_like.py COPYONLY)
configure_file(bextract.py ${CMAKE_CURRENT_BINARY_DIR}/bextract.py COPYONLY)


SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})
SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(marsyas.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(marsyas.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(marsyas python  marsyas.i)
SWIG_LINK_LIBRARIES(marsyas marsyas ${PYTHON_LIBRARIES} ${OS_LIBS})

# The installation directory for the Python modules.
# Get the default directory from distutils.
execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print sysconfig.get_python_lib(1, 0)"
  OUTPUT_VARIABLE PYTHON_INSTDIR_DEFAULT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# User can override the default through the CMake UI.
set(
  PYTHON_INSTDIR
  ${PYTHON_INSTDIR_DEFAULT}
  CACHE STRING "The installation directory for the Python modules."
)

install(TARGETS _marsyas LIBRARY DESTINATION ${PYTHON_INSTDIR})


install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/marsyas.py 
  DESTINATION ${PYTHON_INSTDIR}
  )
