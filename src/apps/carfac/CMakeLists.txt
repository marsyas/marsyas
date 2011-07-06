set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} m)

set(APP
	carfac_hacking
)
set(APP_SOURCES
	carfac_hacking.cpp
)

set(APP2
	carfac_profiling
)
set(APP2_SOURCES
	carfac_profiling.cpp
)

set(APP3
	carfac_testing
)
set(APP3_SOURCES
	carfac_testing.cpp
)

marsyas_app(${APP} "${APP_SOURCES}")
marsyas_app(${APP2} "${APP2_SOURCES}")
marsyas_app(${APP3} "${APP3_SOURCES}")

TARGET_LINK_LIBRARIES(carfac_hacking ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
