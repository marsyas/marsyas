SOURCES = testTrans.cpp
HEADERS += Transcriber.h
SOURCES += Transcriber.cpp

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

