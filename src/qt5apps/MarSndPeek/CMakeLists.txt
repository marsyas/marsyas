set(APP
  MarSndPeek
)

set(APP_SOURCES
  main.cpp
  glwidget.cpp
  window.cpp
)

set(APP_HEADERS
  glwidget.h
  window.h
)

qt5_wrap_cpp( APP_MOC_SRC ${APP_HEADERS} )

marsyas_qt5_app( ${APP} ${APP_SOURCES} ${APP_MOC_SRC} )

qt5_use_modules( ${APP} Widgets OpenGL )
target_link_libraries( ${APP} ${OPENGL_LIBRARIES} )
