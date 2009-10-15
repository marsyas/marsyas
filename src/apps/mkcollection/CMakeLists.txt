set(APP
	mkcollection
)
set(APP_SOURCES
	mkcollection.cpp
)

if (MARSYAS_STATIC) 
   marsyas_static_app(${APP} "${APP_SOURCES}")
else (MARSYAS_STATIC)
   marsyas_app(${APP} "${APP_SOURCES}")
endif (MARSYAS_STATIC)



