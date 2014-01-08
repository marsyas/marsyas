set(APP
  MarLpc
)

set(APP_SOURCES 
  MarLpcWindow.cpp
  main.cpp
)

set(APP_HEADERS 
  MarLpcWindow.h
  realvec_edit.h
)

qt5_wrap_cpp( APP_MOC_SRC ${APP_HEADERS} )

marsyas_qt5_app( ${APP} ${APP_SOURCES} ${APP_MOC_SRC} )

qt5_use_modules( ${APP} Widgets )
