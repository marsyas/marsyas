set(APP
	MarMonitors
)

set(APP_SOURCES 
	MarMonitors.cpp
	main.cpp
)

set(APP_HEADERS 
	MarMonitors.h 
)

set(APP_UI
)


set(APP_RESOURCES
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

