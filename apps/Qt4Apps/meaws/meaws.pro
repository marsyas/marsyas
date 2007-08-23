#include (../Qt4Apps.pri)

SOURCES = start.cpp mainwindow.cpp
HEADERS = defs.h mainwindow.h

HEADERS += user.h exerciseDispatcher.h
SOURCES += user.cpp exerciseDispatcher.cpp

HEADERS += exerciseAbstract.h exerciseIntonation.h exerciseControl.h
SOURCES += exerciseAbstract.cpp exerciseIntonation.cpp exerciseControl.cpp

HEADERS += exerciseShift.h
SOURCES += exerciseShift.cpp

HEADERS += backend.h
SOURCES += backend.cpp

#temporary?  maybe?
HEADERS += ../QtMarPlot.h
SOURCES += ../QtMarPlot.cpp

#HEADERS += ../MarSystemQtWrapper.h
#SOURCES += ../MarSystemQtWrapper.cpp


RESOURCES = icons.qrc
TARGET = meaws
VERSION = 0.5
#CONFIG = qt


MARSYAS_INSTALL_DIR = ../../../
#MARSYAS_INSTALL_DIR = ${HOME}/usr/
#MARSYAS_INSTALL_DIR = /usr/local
message("Marsyas was installed in $$MARSYAS_INSTALL_DIR, right?")


unix {
  MEAWS_DATADIR = $$(HOME)/.meaws/
#  DEFINES += UNIX
}
win32{
  MEAWS_DATADIR = c:\temp\meaws
#  DEFINES += WIN32
}
message("Installing data to $$MEAWS_DATADIR")
data.path = $$MEAWS_DATADIR
data.files = data/
INSTALLS += data

INCLUDEPATH += ../../../lib/release/

INCLUDEPATH += $$MARSYAS_INSTALL_DIR/marsyas/
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/release

unix:LIBS += -lmarsyas -lmarqtactual -L$$MARSYAS_INSTALL_DIR/lib
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation



win32 {
	INCLUDEPATH += ../../../lib/release/
	INCLUDEPATH += . ..
	CONFIG += console
}


