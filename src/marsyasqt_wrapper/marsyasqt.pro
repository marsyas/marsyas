TEMPLATE = lib
CONFIG += staticlib
TARGET = marsyasqt

HEADERS = MarSystemQtWrapper.h
SOURCES = MarSystemQtWrapper.cpp


DESTDIR = $$quote( "$$BASEDIR/lib/release" )
#CONFIG(release, debug|release) {
#  message(Building with release support.)
#  DESTDIR = $$quote( "$$BASEDIR/lib/release/" )
#}

#CONFIG(debug, debug|release) {
#  message(Building with debug support.)
#  DESTDIR = $$quote( "$$BASEDIR/lib/debug/" )
#}


INCLUDEPATH += $$BASEDIR/src/marsyas/
LIBS += -lmarsyas -L$$BASEDIR/lib/release/


