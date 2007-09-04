TEMPLATE = lib
CONFIG += staticlib
TARGET = marsyasqt_wrapper

INCLUDEPATH += $$quote( "$$BASEDIR/src/marsyas" )

HEADERS = MarSystemQtWrapper.h
SOURCES = MarSystemQtWrapper.cpp
HEADERS += QtMarPlot.h
SOURCES += QtMarPlot.cpp

CONFIG(release, debug|release) {
  message(Building with release support.)
  DESTDIR = $$quote( "$$BASEDIR/lib/release/" )
}

CONFIG(debug, debug|release) {
  message(Building with debug support.)
  DESTDIR = $$quote( "$$BASEDIR/lib/debug/" )
}





