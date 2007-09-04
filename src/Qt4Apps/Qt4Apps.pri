include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

CONFIG *= qt   # Qt4Apps need Qt

###############################################
# 			  Destination dirs
###############################################
CONFIG(release, debug|release) {
  message(Building with release support.)
  DESTDIR = $$quote( "$$BASEDIR/bin/release" )
}
CONFIG(debug, debug|release) {
  message(Building with debug support.)
  DESTDIR = $$quote( "$$BASEDIR/bin/debug" )
}

INCLUDEPATH += $$quote( "$$BASEDIR/src/marsyasqt_wrapper" )

CONFIG(release, debug|release){
	win32:LIBS 	+= 	marsyas.lib marsyasqt_wrapper.lib
	!win32:LIBS += 	-lmarsyas -lmarsyasqt_wrapper
	LIBPATH += $$quote( "$$BASEDIR/lib/release" )
}
CONFIG(debug, debug|release){
	CONFIGURE	+= console
	win32:LIBS 	+= 	marsyas.lib marsyasqt_wrapper.lib
	!win32:LIBS += 	-lmarsyas -lmarsyasqt_wrapper
	LIBPATH += $$quote( "$$BASEDIR/lib/debug" )
}


