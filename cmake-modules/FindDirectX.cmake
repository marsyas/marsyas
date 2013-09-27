
if(NOT WIN32)
  set(DIRECTX_FOUND FALSE)
  return()
endif()

include(FindPackageHandleStandardArgs)

find_path(DSOUND_INCLUDE_DIR dsound.h
  PATHS
    "$ENV{DXSDK_DIR}/Include"
    "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
    "$ENV{PROGRAMFILES}/Microsoft DirectX SDK (August 2008)/Include"
    "$ENV{PROGRAMFILES}/Microsoft DirectX SDK (June 2010)/Include"
  DOC "The directory where dsound.h resides"
)
mark_as_advanced(DSOUND_INCLUDE_DIR)

find_library(DSOUND_LIBRARY dsound
  PATHS
    "$ENV{DXSDK_DIR}/Lib/x86"
    "$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
    "$ENV{PROGRAMFILES}/Microsoft DirectX SDK (August 2008)/Lib/x86"
    "$ENV{PROGRAMFILES}/Microsoft DirectX SDK (June 2010)/Lib/x86"
  DOC "The directory where the dsound library resides"
)
mark_as_advanced(DSOUND_LIBRARY)

find_package_handle_standard_args(DirectX DEFAULT_MSG DSOUND_LIBRARY DSOUND_INCLUDE_DIR)
