# - Find python interpreter
# This module finds if Python interpreter is installed and determines where the
# executables are. This code sets the following variables:
#
#  PythonInterp_FOUND - Was the Python executable found
#  PythonInterp_EXECUTABLE - path to the Python interpreter
#  PythonInterp_VERSION - version of the Python interpreter
#
#  PYTHON_EXECUTABLE  - Same as PythonInterp_EXECUTABLE, kept for compatibility
#  PYTHONINTERP_FOUND - Same as PythonInterp_FOUND, kept for compatibility
#

FIND_PROGRAM(PythonInterp_EXECUTABLE
  NAMES python2.7 python2.6 python2.5 python2.4 python2.3 python2.2 python2.1 python2.0 python1.6 python1.5 python
  PATHS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.7\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.6\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.5\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.4\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.2\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.1\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.0\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.6\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.5\\InstallPath]

  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.7\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.6\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.5\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.4\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.2\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.1\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\2.0\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\1.6\\InstallPath]
  [HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\1.5\\InstallPath]

  )
MARK_AS_ADVANCED(PythonInterp_EXECUTABLE)
SET(PYTHON_EXECUTABLE ${PythonInterp_EXECUTABLE})

SET(PythonInterp_FOUND)
IF(PythonInterp_EXECUTABLE)
  SET(PythonInterp_FOUND ON)
  SET(PythonInterp_VERSION "NO" STRING DOCSTRING "Python version")
  EXECUTE_PROCESS(COMMAND ${PythonInterp_EXECUTABLE}
    -c  "import sys; print sys.version"
    OUTPUT_VARIABLE PYTHONINTERP_version_output
    ERROR_VARIABLE PYTHONINTERP_version_error
    RESULT_VARIABLE PYTHONINTERP_version_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  IF(NOT ${PYTHONINTERP_version_result} EQUAL 0)
    MESSAGE(SEND_ERROR "Command '${PythonInterp_EXECUTABLE} -c  \"import sys; print sys.version\" failed with output:\n${PythonInterp_EXECUTABLE_version_error}")
  ELSE(NOT ${PYTHONINTERP_version_result} EQUAL 0)
    STRING(REGEX REPLACE "^([0-9.]+).*" "\\1"
      PythonInterp_VERSION "${PYTHONINTERP_version_output}")
  ENDIF(NOT ${PYTHONINTERP_version_result} EQUAL 0)
ENDIF(PythonInterp_EXECUTABLE)


