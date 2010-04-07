# - this module looks for Matlab
# Defines:
#  MATLAB_INCLUDE_DIR: include path for mex.h, engine.h
#  MATLAB_LIBRARIES:   required libraries: libmex, etc
#  MATLAB_MEX_LIBRARY: path to libmex.lib
#  MATLAB_MX_LIBRARY:  path to libmx.lib
#  MATLAB_ENG_LIBRARY: path to libeng.lib

SET(MATLAB_FOUND 0)




IF(WIN32)
  IF(${CMAKE_GENERATOR} MATCHES "Visual Studio 6")
    SET(MATLAB_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MathWorks\\MATLAB\\7.0;MATLABROOT]/extern/lib/win32/microsoft/msvc60")
  ELSE(${CMAKE_GENERATOR} MATCHES "Visual Studio 6")
    IF(${CMAKE_GENERATOR} MATCHES "Visual Studio 7")
      # Assume people are generally using 7.1,
      # if using 7.0 need to link to: ../extern/lib/win32/microsoft/msvc70
      SET(MATLAB_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MathWorks\\MATLAB\\7.0;MATLABROOT]/extern/lib/win32/microsoft/msvc71")
    ELSE(${CMAKE_GENERATOR} MATCHES "Visual Studio 7")
      IF(${CMAKE_GENERATOR} MATCHES "Borland")
        # Same here, there are also: bcc50 and bcc51 directories
        SET(MATLAB_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MathWorks\\MATLAB\\7.0;MATLABROOT]/extern/lib/win32/microsoft/bcc54")
      ELSE(${CMAKE_GENERATOR} MATCHES "Borland")
        IF(MATLAB_FIND_REQUIRED)
          MESSAGE(FATAL_ERROR "Generator not compatible: ${CMAKE_GENERATOR}")
        ENDIF(MATLAB_FIND_REQUIRED)
      ENDIF(${CMAKE_GENERATOR} MATCHES "Borland")
    ENDIF(${CMAKE_GENERATOR} MATCHES "Visual Studio 7")
  ENDIF(${CMAKE_GENERATOR} MATCHES "Visual Studio 6")
  FIND_LIBRARY(MATLAB_MEX_LIBRARY
    libmex
    ${MATLAB_ROOT}
    )
  FIND_LIBRARY(MATLAB_MX_LIBRARY
    libmx
    ${MATLAB_ROOT}
    )
  FIND_LIBRARY(MATLAB_ENG_LIBRARY
    libeng
    ${MATLAB_ROOT}
    )

  FIND_PATH(MATLAB_INCLUDE_DIR
    "mex.h"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MathWorks\\MATLAB\\7.0;MATLABROOT]/extern/include"
    )
ENDIF (WIN32)


IF( UNIX )
	IF (APPLE) 
    SET(MATLAB_ROOT
      /Applications/MATLAB74/bin/maci
      /Applications/MATLAB_R2009a.app/bin/maci
      $ENV{HOME}/MATLAB74/bin/maci
      $ENV{MATLAB}/bin/maci
      )
	
 	FIND_LIBRARY(MATLAB_MEX_LIBRARY
		mex
		PATHS    	${MATLAB_ROOT}
	  NO_DEFAULT_PATH	
		    )


    FIND_LIBRARY(MATLAB_MX_LIBRARY
  	  mx
	  PATHS    ${MATLAB_ROOT}
	  NO_DEFAULT_PATH
    ) 


  FIND_LIBRARY(MATLAB_ENG_LIBRARY
      eng
	  PATHS ${MATLAB_ROOT}
	  NO_DEFAULT_PATH
  )

 FIND_LIBRARY(MATLAB_UT_LIBRARY
		ut
		PATHS	${MATLAB_ROOT}
	  NO_DEFAULT_PATH
 )



 FIND_LIBRARY(MATLAB_MAT_LIBRARY
		mat
		PATHS	${MATLAB_ROOT}
	  NO_DEFAULT_PATH
 )


#  FIND_LIBRARY(MATLAB_ICUDATA_LIBRARY
#    icudata
#		PATHS	${MATLAB_ROOT}
#	  NO_DEFAULT_PATH
#
#    )


#  FIND_LIBRARY(MATLAB_ICUUC_LIBRARY
#    icuuc
#		PATHS	${MATLAB_ROOT}
#	  NO_DEFAULT_PATH
#
#    )


#  FIND_LIBRARY(MATLAB_ICUI18N_LIBRARY
#    icui18n
#		PATHS	${MATLAB_ROOT}
#	  NO_DEFAULT_PATH
#
#    )



#  FIND_LIBRARY(MATLAB_ICUIO_LIBRARY
#    icuio
#		PATHS	${MATLAB_ROOT}
#	  NO_DEFAULT_PATH
#
#    )

#  FIND_LIBRARY(MATLAB_Z_LIBRARY
#    z
#		PATHS	${MATLAB_ROOT}
#	  NO_DEFAULT_PATH
#
#    )


  FIND_LIBRARY(MATLAB_HDF5_LIBRARY
    hdf5.5
		PATHS	${MATLAB_ROOT}
	  NO_DEFAULT_PATH

    )





  	FIND_PATH(MATLAB_INCLUDE_DIR
    "mex.h"
      /Applications/MATLAB74/extern/include
      /Applications/MATLAB_R2009a.app/extern/include
      $ENV{HOME}/MATLAB74/extern/include
      $ENV{MATLAB}/extern/include
    )








	ELSE(APPLE)  

  IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # Regular x86
    SET(MATLAB_ROOT
      /usr/local/matlab-7sp1/bin/glnx86/
      /opt/matlab-7sp1/bin/glnx86/
      $ENV{HOME}/matlab-7sp1/bin/glnx86/
      $ENV{HOME}/redhat-matlab/bin/glnx86/
      )
  ELSE(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # AMD64:
    SET(MATLAB_ROOT
      /usr/local/matlab-7sp1/bin/glnxa64/
      /opt/matlab-7sp1/bin/glnxa64/
      $ENV{HOME}/matlab7_64/bin/glnxa64/
      $ENV{HOME}/matlab-7sp1/bin/glnxa64/
      $ENV{HOME}/redhat-matlab/bin/glnxa64/
      )
  ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
  FIND_LIBRARY(MATLAB_MEX_LIBRARY
    mex
    ${MATLAB_ROOT}
    )
  FIND_LIBRARY(MATLAB_MX_LIBRARY
    mx
    ${MATLAB_ROOT}
    )
  FIND_LIBRARY(MATLAB_ENG_LIBRARY
    eng
    ${MATLAB_ROOT}
    )


  FIND_PATH(MATLAB_INCLUDE_DIR
    "mex.h"
    "/usr/local/matlab-7sp1/extern/include/"
    "/opt/matlab-7sp1/extern/include/"
    "$ENV{HOME}/matlab-7sp1/extern/include/"
    "$ENV{HOME}/redhat-matlab/extern/include/"
    )
	ENDIF(APPLE)
ENDIF(UNIX)

# This is common to UNIX and Win32:
SET(MATLAB_LIBRARIES
  ${MATLAB_ENG_LIBRARY}
  ${MATLAB_MX_LIBRARY}
  ${MATLAB_UT_LIBRARY}
  ${MATLAB_MAT_LIBRARY} 
  ${MATLAB_ICUDATA_LIBRARY} 
  ${MATLAB_ICUUC_LIBRARY} 
  ${MATLAB_ICUI18N_LIBRARY} 
  ${MATLAB_ICUIO_LIBRARY} 
  ${MATLAB_Z_LIBRARY}
  ${MATLAB_HDF5_LIBRARY}
)







# IF(MATLAB_INCLUDE_DIR AND MATLAB_LIBRARIES)
IF(MATLAB_INCLUDE_DIR)
  SET(MATLAB_FOUND 1)
  MESSAGE(STATUS "MATLAB found configuring Engine Support") 
# ENDIF(MATLAB_INCLUDE_DIR AND MATLAB_LIBRARIES)
ENDIF(MATLAB_INCLUDE_DIR)

MARK_AS_ADVANCED(
  MATLAB_LIBRARIES
  MATLAB_MEX_LIBRARY
  MATLAB_MX_LIBRARY
  MATLAB_ENG_LIBRARY
  MATLAB_UT_LIBRARY
  MATLAB_INCLUDE_DIR
  MATLAB_FOUND
  MATLAB_ROOT
)

