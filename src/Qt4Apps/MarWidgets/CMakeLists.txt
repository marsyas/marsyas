set(APP
	MarWidgets
)

set(APP_SOURCES 
	MarLineEdit.cpp
	MarSlider.cpp
	MarSpinBox.cpp
	MarControlWidget.cpp
	MarGLWidget.cpp
	GlWidget.cpp
	MarWidgets.cpp
	main.cpp
)

set(APP_HEADERS 
	GlWidget.h 
	MarControlWidget.h 
	MarGLWidget.h 
	MarLineEdit.h 
	MarSlider.h 
	MarSpinBox.h 
	MarWidgets.h 

)

set(APP_UI
)


set(APP_RESOURCES
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

