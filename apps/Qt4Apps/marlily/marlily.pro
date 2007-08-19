include ( ../Qt4Apps.pri )

SOURCES = testTrans.cpp
#SOURCES = start.cpp
#SOURCES = temp.cpp

HEADERS += mainwindow.h
SOURCES += mainwindow.cpp

HEADERS += Transcriber.h TranscriberExtract.h
SOURCES += Transcriber.cpp TranscriberExtract.cpp

HEADERS += Easymar.h
SOURCES += Easymar.cpp

HEADERS += ../QtMarPlot.h
SOURCES += ../QtMarPlot.cpp

INCLUDEPATH += ../../../lib/release/

unix {
!macx {
	LIBS += -lmarsyas -L../../../lib/release/
	LIBS += -lasound
}
}

macx {
	LIBS += -lmarsyas -L../../../lib/release/
	LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation
}

win32 {
	INCLUDEPATH += ../../../lib/release/
	INCLUDEPATH += . ..
	CONFIG += console
}

