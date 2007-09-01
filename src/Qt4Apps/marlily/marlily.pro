#include ( ../Qt4Apps.pri )

#SOURCES = testShift.cpp
SOURCES = testTrans.cpp
#SOURCES = start.cpp
#SOURCES = temp.cpp

#HEADERS += mainwindow.h
#SOURCES += mainwindow.cpp

HEADERS += Transcriber.h
SOURCES += Transcriber.cpp

#HEADERS += TranscriberExtract.h
#SOURCES += TranscriberExtract.cpp

#HEADERS += Easymar.h
#SOURCES += Easymar.cpp

#HEADERS += ../QtMarPlot.h
#SOURCES += ../QtMarPlot.cpp

DEPTH = ../../../

win32 {
	INCLUDEPATH += ../../../lib/release/
	INCLUDEPATH += . ..
	CONFIG += console
}

#  these files are common to every Marsyas/QT app 
#  you will need to update these paths (and the paths in backend.h)
#  if you copy this directory elsewhere.
#HEADERS += $$DEPTH/apps/Qt4Apps/MarSystemQtWrapper.h
#SOURCES += $$DEPTH/apps/Qt4Apps/MarSystemQtWrapper.cpp


#MARSYAS_INSTALL_DIR  = ${HOME}/usr/
MARSYAS_INSTALL_DIR  = /usr/local
message("If Marsyas was installed, it went into $$MARSYAS_INSTALL_DIR, right?")
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/include/marsyas/
# in case you have not installed Marsyas
INCLUDEPATH += $$DEPTH/marsyas/
LIBPATH += $$DEPTH/lib/release/


unix:LIBS += -lmarsyas -L$$MARSYAS_INSTALL_DIR/lib # -lmad -lvorbis -lvorbisfile
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

