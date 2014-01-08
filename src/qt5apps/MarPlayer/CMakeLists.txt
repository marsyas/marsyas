set(APP
  MarPlayer
)

set(APP_SOURCES
  MarPlayer.cpp
  Mapper.cpp
  main.cpp
)

set(APP_HEADERS
  MarPlayer.h
  Mapper.h
)

set(APP_UI
  MarPlayerForm.ui
)

set(APP_RESOURCES
  MarPlayerForm.qrc
)

qt5_wrap_cpp( APP_MOC_SRC ${APP_HEADERS} )
qt5_wrap_ui( APP_UI_HDR ${APP_UI} )
qt5_add_resources( APP_RESOURCES_SRC ${APP_RESOURCES} )

marsyas_qt5_app( ${APP} ${APP_SOURCES} ${APP_MOC_SRC} ${APP_RESOURCES_SRC} ${APP_UI_HDR} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ) # for UI headers

qt5_use_modules( ${APP} Widgets )
