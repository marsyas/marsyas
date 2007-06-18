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

CONFIG(release, debug|release){
	win32:LIBS 	+= 	marsyas.lib
	!win32:LIBS += 	-lmarsyas 	
	LIBPATH += $$quote( "$$BASEDIR/lib/release" )
}
CONFIG(debug, debug|release){
	win32:LIBS 	+= 	marsyas.lib
	!win32:LIBS += 	-lmarsyas 	
	LIBPATH += $$quote( "$$BASEDIR/lib/debug" )
}