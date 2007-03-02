include( "$$BASEDIR"/marsyasConfig.pri )

CONFIG(release, debug|release) {
  message(Building with release support.)
  DESTDIR = "$$BASEDIR"/bin/release
}

CONFIG(debug, debug|release) {
  message(Building with debug support.)
  DESTDIR = "$$BASEDIR"/bin/debug
}

CONFIG(release, debug|release){
	LIBS 	+= 	-lmarsyas -L\"$$BASEDIR/lib/release\"
}
CONFIG(debug, debug|release){
	LIBS 	+= 	-lmarsyas -L\"$$BASEDIR/lib/debug\"
}

