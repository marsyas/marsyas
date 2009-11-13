set(APP
	MarOscOut
)

set(APP_SOURCES 
	main.cpp
	MarOscOutWindow.cpp
)

set(APP_HEADERS 
	MarOscOutWindow.h
)

marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

