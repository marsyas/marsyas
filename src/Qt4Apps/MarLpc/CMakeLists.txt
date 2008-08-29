set(APP
	MarLpc
)

set(APP_SOURCES 
	MarLpcWindow.cpp
	MarControlGUI.cpp 
	main.cpp
)

set(APP_HEADERS 
	MarLpcWindow.h 
	MarControlGUI.h
)

set(APP_UI
)


set(APP_RESOURCES
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

