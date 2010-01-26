set(APP
	Marx2DGraph
)

set(APP_SOURCES 
	TopPanel.cpp
	main.cpp
)

set(APP_HEADERS 
	TopPanel.h 
)

set(APP_UI
)


set(APP_RESOURCES
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

