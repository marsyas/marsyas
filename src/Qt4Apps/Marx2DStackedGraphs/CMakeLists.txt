set(APP
	Marx2DStackedGraphs
)

set(APP_SOURCES 
	Marx2DStackedGraphs.cpp
	main.cpp
)

set(APP_HEADERS 
	Marx2DStackedGraphs.h 
)

set(APP_UI
)


set(APP_RESOURCES
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

