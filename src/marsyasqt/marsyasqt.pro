include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

TEMPLATE = lib
CONFIG += staticlib
CONFIG += Qt  
QT += network
TARGET = marsyasqt

INCLUDEPATH += $$quote( "$$BASEDIR/src/marsyas" )

HEADERS += MarSystemQtWrapper.h \
	QtMarPlot.h \
	OscMapper.h \
	qoscclient.h \
	qoscserver.h \
	qosctypes.h \
	Marx2DGraph.h

SOURCES += MarSystemQtWrapper.cpp \
	QtMarPlot.cpp \
	OscMapper.cpp \
	qoscclient.cpp \
	qoscserver.cpp \
	qosctypes.cpp \
	Marx2DGraph.cpp


CONFIG(release, debug|release) {
  message(Building with release support.)
  DESTDIR = $$quote( "$$BASEDIR/lib/release/" )
}

CONFIG(debug, debug|release) {
  message(Building with debug support.)
  DESTDIR = $$quote( "$$BASEDIR/lib/debug/" )
}

CONFIG(release, debug|release){
	win32-msvc2005: LIBS  += marsyas.lib
	!win32-msvc2005: LIBS += -lmarsyas
	LIBPATH +=  $$quote( "$$BASEDIR/lib/release" )
}
CONFIG(debug, debug|release){
	win32-msvc2005:LIBS 	+= 	marsyas.lib
	!win32-msvc2005: += -lmarsyas 	
	LIBPATH += $$quote( "$$BASEDIR/lib/debug" )
}






