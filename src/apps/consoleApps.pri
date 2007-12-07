include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

macx:CONFIG -= app_bundle

CONFIG(release, debug|release) {
  message(Building with release support.)
  DESTDIR = $$quote( "$$BASEDIR/bin/release" )
}

CONFIG(debug, debug|release) {
  message(Building with debug support.)
  DESTDIR = $$quote( "$$BASEDIR/bin/debug" )
}

CONFIG(release, debug|release){
	win32-msvc2005: LIBS  += marsyas.lib
	!win32-msvc2005: LIBS += -lmarsyas
	LIBPATH +=  $$quote( \"$$BASEDIR/lib/release\" )
	PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/release/marsyas.lib\" )
}
CONFIG(debug, debug|release){
	win32-msvc2005:LIBS 	+= 	marsyas.lib
	!win32-msvc2005: += -lmarsyas 	
	LIBPATH += $$quote( \"$$BASEDIR/lib/debug\" )
	PRE_TARGETDEPS += $$quote( \"$$BASEDIR/lib/debug/marsyas.lib\" )
}



