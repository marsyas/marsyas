set(APP
	MarGrid
)

set(APP_SOURCES 
	MarGrid.cpp
	ColorMap.cpp
	GreyScaleColormap.cpp
	MarGridWindow.cpp
	SpectraColormap.cpp
	main.cpp
)

set(APP_HEADERS 
	MarGrid.h
	MarGridWindow.h
)

marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

