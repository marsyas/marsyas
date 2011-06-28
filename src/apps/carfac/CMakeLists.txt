set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} m)

set(APP
	carfac_hacking
)
set(APP_SOURCES
	carfac_hacking.cpp
)

marsyas_app(${APP} "${APP_SOURCES}")
TARGET_LINK_LIBRARIES(carfac_hacking ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
