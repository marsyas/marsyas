IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   INCLUDE(FindPkgConfig)
   pkg_check_modules(GSTREAMER gstreamer-0.10 gstreamer-app-0.10)
ENDIF (NOT WIN32)
