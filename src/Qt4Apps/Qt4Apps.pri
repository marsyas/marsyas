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

INCLUDEPATH += $$quote( "$$BASEDIR/src/marsyasqt" )

CONFIG(release, debug|release){
	win32-msvc2005:LIBS 	+= 	marsyas.lib marsyasqt.lib
	!win32-msvc2005:LIBS = 	-lmarsyas -lmarsyasqt $$LIBS
	LIBPATH += $$quote( \"$$BASEDIR/lib/release\" )
	win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/release/marsyas.lib\" )
	win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/release/marsyasqt.lib\" )
	!win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/release/libmarsyas.a\" )
	!win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/release/libmarsyasqt.a\" )
}
CONFIG(debug, debug|release){
	CONFIGURE	+= console
	win32-msvc2005:LIBS 	+= 	marsyas.lib marsyasqt.lib
	!win32-msvc2005:LIBS = 	-lmarsyas -lmarsyasqt $$LIBS
	LIBPATH += $$quote( \"$$BASEDIR/lib/debug\" )

	win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/debug/marsyas.lib\" )
	win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/debug/marsyasqt.lib\" )
	!win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/debug/libmarsyas.a\" )
	!win32-msvc2005:PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/debug/libmarsyasqt.a\" )
}


