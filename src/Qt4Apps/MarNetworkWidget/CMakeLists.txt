set(APP
	MarNetworkWidget
)

set(APP_SOURCES 
	GWidget.cpp 
	MarSystem.cpp 
	GMarSystem.cpp 
	MarNetworkCanvas.cpp 
	GTinyButton.cpp 
	main.cpp
)

set(APP_HEADERS 
		GWidget.h 
		MarSystem.h 
		GMarSystem.h 
		MarNetworkCanvas.h 
		GTinyButton.h
)

set(APP_UI
)


set(APP_RESOURCES
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

