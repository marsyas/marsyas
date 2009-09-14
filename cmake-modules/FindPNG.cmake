# - Find the native PNG includes and library
#
# This module defines
#  PNG_INCLUDE_DIR, where to find png.h, etc.
#  PNG_LIBRARIES, the libraries to link against to use PNG.
#  PNG_DEFINITIONS - You should ADD_DEFINITONS(${PNG_DEFINITIONS}) before compiling code that includes png library files.
#  PNG_FOUND, If false, do not try to use PNG. Aalso defined, but not for general use are
#  PNG_LIBRARY, where to find the PNG library.
#  None of the above will be defined unles zlib can be found.
#  PNG depends on Zlib
	
# - Find zlib
# Find the native ZLIB includes and library
#
#  ZLIB_INCLUDE_DIR - where to find zlib.h, etc.
#  ZLIB_LIBRARIES   - List of libraries when using zlib.
#  ZLIB_FOUND       - True if zlib found.


IF (ZLIB_INCLUDE_DIR)
  # Already in cache, be silent
  SET(ZLIB_FIND_QUIETLY TRUE)
ENDIF (ZLIB_INCLUDE_DIR)

FIND_PATH(ZLIB_INCLUDE_DIR zlib.h)

SET(ZLIB_NAMES z zlib zdll)
FIND_LIBRARY(ZLIB_LIBRARY NAMES ${ZLIB_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZLIB DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE_DIR)

IF(ZLIB_FOUND)
  SET( ZLIB_LIBRARIES ${ZLIB_LIBRARY} )
ELSE(ZLIB_FOUND)
  SET( ZLIB_LIBRARIES )
ENDIF(ZLIB_FOUND)

MARK_AS_ADVANCED( ZLIB_LIBRARY ZLIB_INCLUDE_DIR )
	IF(ZLIB_FOUND)
	  FIND_PATH(PNG_PNG_INCLUDE_DIR png.h
	  $ENV{LIBPNGDIR}/include
	  /sw/include
	  /usr/local/include
	  /usr/include
	  /opt/local/include
	  )
	
	  SET(PNG_NAMES ${PNG_NAMES} png libpng)
	  FIND_LIBRARY(PNG_LIBRARY
	    NAMES ${PNG_NAMES}
	    PATHS
	    $ENV{LIBPNGDIR}/lib
	    /usr/lib
	    /usr/local/lib
	  )
	
	  IF (PNG_LIBRARY AND PNG_PNG_INCLUDE_DIR)
	      # png.h includes zlib.h. Sigh.
	      SET(PNG_INCLUDE_DIR ${PNG_PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} )
	      SET(PNG_LIBRARIES ${PNG_LIBRARY} ${ZLIB_LIBRARY})
	      SET(PNG_FOUND "YES")
	
	      IF (CYGWIN)
	        IF(BUILD_SHARED_LIBS)
	           # No need to define PNG_USE_DLL here, because it's default for Cygwin.
	        ELSE(BUILD_SHARED_LIBS)
	          SET (PNG_DEFINITIONS -DPNG_STATIC)
	        ENDIF(BUILD_SHARED_LIBS)
	      ENDIF (CYGWIN)
	
	  ENDIF (PNG_LIBRARY AND PNG_PNG_INCLUDE_DIR)
	
	ENDIF(ZLIB_FOUND)
	
	IF (APPLE)
	    #SET(PNG_INCLUDE_DIR "/opt/local/include" )
	    SET(PNG_LIBRARIES "" )
	    #SET(PNG_LIBRARY_STATIC "/opt/local/lib/libpng12.a")
	        FIND_LIBRARY(PNG_LIBRARY_STATIC NAMES libpng12.a PATHS /opt/local/lib /sw/lib)
	    SET(PNG_FOUND "YES")
	ENDIF (APPLE)
	
	MARK_AS_ADVANCED(PNG_PNG_INCLUDE_DIR PNG_LIBRARY PNG_LIBRARY_STATIC )
