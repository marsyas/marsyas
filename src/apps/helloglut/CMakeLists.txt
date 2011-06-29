set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} m)

set(APP
	helloglut
)
set(APP_SOURCES
	helloglut.cpp
)

marsyas_app(${APP} "${APP_SOURCES}")

TARGET_LINK_LIBRARIES(helloglut ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
