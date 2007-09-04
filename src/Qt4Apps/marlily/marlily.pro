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

DEPTH = ../../../

win32 {
	INCLUDEPATH += ../../../lib/release/
	INCLUDEPATH += . ..
	CONFIG += console
}


#  update this junk from  src/examples/Qt4tutorial

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

