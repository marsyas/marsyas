set(APP
	MarOscIn
)

set(APP_SOURCES 
	main.cpp
	MarOscInWindow.cpp
)

set(APP_HEADERS 
	MarOscInWindow.h
)

marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

