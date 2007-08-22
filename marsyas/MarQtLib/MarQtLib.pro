TEMPLATE = lib
CONFIG += staticlib
TARGET = marqtlib

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


DEPTH = ../../
INCLUDEPATH += $$DEPTH/marsyas/
LIBS += -lmarsyas -L$$DEPTH/lib/release/



