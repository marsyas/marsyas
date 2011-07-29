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

set(APP4
	carfac_binaural
)
set(APP4_SOURCES
	carfac_binaural.cpp
)

set(APP5
	carfac_png
)
set(APP5_SOURCES
	carfac_png.cpp
)

marsyas_app(${APP} "${APP_SOURCES}")
marsyas_app(${APP2} "${APP2_SOURCES}")
marsyas_app(${APP3} "${APP3_SOURCES}")
marsyas_app(${APP4} "${APP4_SOURCES}")
marsyas_app(${APP5} "${APP5_SOURCES}")

TARGET_LINK_LIBRARIES(carfac_hacking ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
TARGET_LINK_LIBRARIES(carfac_binaural ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
