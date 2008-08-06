set(APP
	MarPlayer
)

set(APP_SOURCES 
	MarPlayer.cpp
	Mapper.cpp 
	main.cpp
)

set(APP_HEADERS 
	MarPlayer.h 
	Mapper.h
)

set(APP_UI
	MarPlayerForm.ui
)

set(APP_RESOURCES
	MarPlayerForm.qrc
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

