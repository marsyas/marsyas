set(APP
	speakerSeg
)
set(APP_SOURCES
	speakerSeg.cpp
)

set(APP2
	speakerSeg2
)
set(APP_SOURCES2
	speakerSeg2.cpp
)

marsyas_app(${APP} "${APP_SOURCES}")
marsyas_app(${APP2} "${APP_SOURCES2}")

