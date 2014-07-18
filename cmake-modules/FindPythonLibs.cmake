# - Find python libraries
# This module finds if Python is installed and determines where the
# include files and libraries are. It also determines what the name of
# the library is. This code sets the following variables:
#
#  PYTHON_LIBRARIES     = path to the python library
#  PYTHON_LIBRARIES_DEBUG = path to the python debug library
#  PYTHON_INCLUDE_PATH  = path to where Python.h is found
#

list(FIND PythonLibs_FIND_COMPONENTS DEBUG PYTHON_FIND_DEBUG)
if(PYTHON_FIND_DEBUG EQUAL -1)
  set(PYTHON_FIND_DEBUG FALSE)
else()
  set(PYTHON_FIND_DEBUG TRUE)
endif()

set(PYTHON_SUITABLE_VERSIONS 2.7 2.6 2.5 2.4 2.3 2.2 2.1 2.0 1.6 1.5)

EXECUTE_PROCESS(COMMAND python-config --prefix OUTPUT_VARIABLE PYTHON_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)

if(APPLE)
  include(CMakeFindFrameworks)
  cmake_find_frameworks(Python)
endif()

foreach(PYTHON_VERSION ${PYTHON_SUITABLE_VERSIONS})
  string(REPLACE "." "" PYTHON_VERSION_DOTLESS ${PYTHON_VERSION})

  set(PYTHON_INCLUDE_SEARCH_PATHS ${PYTHON_PREFIX}/include)
  set(PYTHON_LIB_SEARCH_PATHS ${PYTHON_PREFIX}/lib)

  if(WIN32)
    get_filename_component(PYTHON_INSTALL_PATH [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${PYTHON_VERSION}\\InstallPath] ABSOLUTE)
    if(PYTHON_INSTALL_PATH)
      list(APPEND PYTHON_INCLUDE_SEARCH_PATHS "${PYTHON_INSTALL_PATH}/include")
      list(APPEND PYTHON_LIB_SEARCH_PATHS "${PYTHON_INSTALL_PATH}/libs" "${PYTHON_INSTALL_PATH}/libs/Debug")
    endif()
  elseif(APPLE AND Python_FRAMEWORKS)
    foreach(dir ${Python_FRAMEWORKS})
      list(APPEND PYTHON_INCLUDE_SEARCH_PATHS "${dir}/Versions/${PYTHON_VERSION}/include/python${PYTHON_VERSION}")
    endforeach()
  endif()

  find_library(PYTHON_LIBRARY
    NAMES
      python${PYTHON_VERSION}
      python${PYTHON_VERSION_DOTLESS}
    PATHS
      ${PYTHON_LIB_SEARCH_PATHS}
    PATH_SUFFIXES
      python${PYTHON_VERSION}/config
  )

  if(WIN32)
    find_library(PYTHON_LIBRARY_DEBUG
      NAMES
        python${PYTHON_VERSION_DOTLESS}_d
      PATHS
        ${PYTHON_LIB_SEARCH_PATHS}
    )
  else()
    set(PYTHON_LIBRARY_DEBUG ${PYTHON_LIBRARY})
  endif()

  find_path(PYTHON_INCLUDE_PATH
    NAMES Python.h
    PATHS
      ${PYTHON_INCLUDE_SEARCH_PATHS}
    PATH_SUFFIXES
      python${PYTHON_VERSION}
  )

  if(PYTHON_LIBRARY AND PYTHON_INCLUDE_PATH)
    break()
  endif()
endforeach() # PYTHON_VERSION


MARK_AS_ADVANCED(
  PYTHON_LIBRARY
  PYTHON_LIBRARY_DEBUG
  PYTHON_INCLUDE_PATH
)

# Python Should be built and installed as a Framework on OSX
IF(Python_FRAMEWORKS)
STRING(REGEX REPLACE "/Python.*" "" FRAMEWORK_PATH ${PYTHON_PREFIX})
  # If a framework has been selected for the include path,
  # make sure "-framework" is used to link it.
  IF("${PYTHON_INCLUDE_PATH}" MATCHES "Python\\.framework")
    SET(PYTHON_LIBRARY "")
  ENDIF("${PYTHON_INCLUDE_PATH}" MATCHES "Python\\.framework")
  IF(NOT PYTHON_LIBRARY)
    SET (PYTHON_LIBRARY "-F${FRAMEWORK_PATH} -framework Python" CACHE FILEPATH "Python Framework" FORCE)
  ENDIF(NOT PYTHON_LIBRARY)
ENDIF(Python_FRAMEWORKS)

if(NOT PYTHON_FIND_DEBUG)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(PythonLibs DEFAULT_MSG PYTHON_LIBRARY PYTHON_INCLUDE_PATH)
else()
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(PythonLibs DEFAULT_MSG PYTHON_LIBRARY_DEBUG PYTHON_INCLUDE_PATH)
endif()

set(PYTHON_LIBRARIES ${PYTHON_LIBRARY})
set(PYTHON_LIBRARIES_DEBUG ${PYTHON_LIBRARY_DEBUG})
set(PYTHON_INCLUDE_DIRS "${PYTHON_INCLUDE_PATH}")
