
add_subdirectory(modules)
add_subdirectory(scripts)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/swig/)


SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})
SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(marsyas.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(marsyas.i PROPERTIES SWIG_FLAGS "-includeall")
SET_SOURCE_FILES_PROPERTIES(../realvec.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(marsyas python marsyas.i ../realvec.i)

SWIG_LINK_LIBRARIES(marsyas marsyas ${PYTHON_LIBRARIES} ${OS_LIBS})



# User can override the default through the CMake UI - commented out
# as it did not work on OS X and macports. 
# if (DEFINED CMAKE_INSTALL_PREFIX)

#   execute_process(
#     COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(0, 0, prefix=''))"
#     OUTPUT_VARIABLE PYTHON_INSTDIR_DEFAULT
#     OUTPUT_STRIP_TRAILING_WHITESPACE
#   )

#   MESSAGE(${PYTHON_INSTDIR_DEFAULT})
#   MESSAGE(${CMAKE_INSTALL_PREFIX})

#   if (MARSYAS_WIN32)
#      set(
#      PYTHON_INSTDIR
#      ${PYTHON_INSTDIR_DEFAULT}
#       CACHE STRING "The installation directory for the Python modules."
#       )
#     else (MARSYAS_WIN32) 
#       set(
#       PYTHON_INSTDIR
#       ${CMAKE_INSTALL_PREFIX}/${PYTHON_INSTDIR_DEFAULT}
#       CACHE STRING "The installation directory for the Python modules."
#       )
#     endif (MARSYAS_WIN32)

# else ()

  # The installation directory for the Python modules.
  # Get the default directory from distutils.
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1, 0, '${CMAKE_INSTALL_PREFIX}'))"
    OUTPUT_VARIABLE PYTHON_INSTDIR_DEFAULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(
    PYTHON_INSTDIR
    ${PYTHON_INSTDIR_DEFAULT}
    CACHE STRING "The installation directory for the Python modules."
  )

# endif()

install(TARGETS _marsyas LIBRARY DESTINATION ${PYTHON_INSTDIR})


install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/marsyas.py
  DESTINATION ${PYTHON_INSTDIR}
  )


 
 
