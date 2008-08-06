set(APP
	MarPhasevocoder
)

set(APP_SOURCES 
	MarPhasevocoderWindow.cpp
	MarControlGUI.cpp 
	main.cpp
)

set(APP_HEADERS 
	MarPhasevocoderWindow.h 
	MarControlGUI.h
)

set(APP_UI
)


set(APP_RESOURCES
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

