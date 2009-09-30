set(APP
	MarOscMultiGain
)

set(APP_SOURCES 
	main.cpp
	MarOscMultiGainWindow.cpp
)

set(APP_HEADERS 
	MarOscMultiGainWindow.h
)

marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

