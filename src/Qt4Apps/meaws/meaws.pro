#include (../Qt4Apps.pri)

# main stuff
SOURCES = start.cpp mainwindow.cpp
HEADERS = defs.h mainwindow.h

# qt objects
HEADERS += QClickFrame.h
SOURCES += QClickFrame.cpp

# meaws top-level objects
HEADERS += user.h dispatcher-exercise.h metro.h
SOURCES += user.cpp dispatcher-exercise.cpp metro.cpp

# audio I/O and feature extraction
HEADERS += backend.h
SOURCES += backend.cpp

# exercises
HEADERS += abstract-exercise.h abstract-try.h
SOURCES += abstract-exercise.cpp abstract-try.cpp

HEADERS += intonation-exercise.h intonation-try.h QtMarIntonationBars.h
SOURCES += intonation-exercise.cpp intonation-try.cpp QtMarIntonationBars.cpp

HEADERS += rhythm-exercise.h rhythm-try.h QtMarRhythmLines.h
SOURCES += rhythm-exercise.cpp rhythm-try.cpp QtMarRhythmLines.cpp


# old exercises, not working in current code
#HEADERS += exerciseShift.h exerciseControl.h
#SOURCES += exerciseShift.cpp exerciseControl.cpp


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

INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyas/
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/marsyas/
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyasqt/  # in svn
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/release
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/

unix:LIBS += -lmarsyas -lmarsyasqt
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation



win32 {
	INCLUDEPATH += ../../../lib/release/
	INCLUDEPATH += . ..
	CONFIG += console
}


