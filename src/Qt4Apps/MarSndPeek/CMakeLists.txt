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

INCLUDE_DIRECTORIES(${QT_QTOPENGL_INCLUDE_DIR})

marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

