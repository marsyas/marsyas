include(FindPackageHandleStandardArgs)

find_path(VAMP_INCLUDE_DIR vamp-sdk/Plugin.h)
find_library(VAMP_LIBRARY NAMES vamp-sdk)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Vamp DEFAULT_MSG VAMP_LIBRARY VAMP_INCLUDE_DIR)
