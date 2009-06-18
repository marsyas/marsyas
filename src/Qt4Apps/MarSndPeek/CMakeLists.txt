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

INCLUDE_DIRECTORIES(${QT_QTOPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
set(LIBS ${LIBS} ${GLUT_LIBRARIES})

marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

TARGET_LINK_LIBRARIES(${APP} ${GLUT_LIBRARIES})
