include (../Qt4Apps.pri)

SOURCES = start.cpp
HEADERS = mainwindow.h backend.h metro.h
SOURCES += mainwindow.cpp backend.cpp metro.cpp
HEADERS += user.h exercise.h analyze.h
SOURCES += user.cpp exercise.cpp analyze.cpp
HEADERS += ../MarSystemQtWrapper.h
SOURCES += ../MarSystemQtWrapper.cpp 
RESOURCES = progdata.qrc music.qrc
TARGET = meaws
VERSION = 0.4
#CONFIG = qt

MUSICDIS = $$DESTDIR
macx:MUSICDIR = $$DESTDIR/meaws.app/Contents/MacOS
MUSICDIR = $$MUSICDIR/music/
#message($$MUSICDIR)
music.path = $$MUSICDIR
music.files = music/*
INSTALLS += music


INCLUDEPATH += ../../../lib/release/


!win32:LIBS += -lmarsyas -L../../../lib/release/

unix {
  !macx {
  LIBS += -lasound
	}
}
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

# needs to be updated:
win32:DEFINES += __WINDOWS_DS__
win32:LIBS += -lmarsyas  -ldsound  
win32:LIBPATH += ..\..\marsyasVisualStudio2005\marsyas\Release "$$(DXSDK_DIR)Lib\x86\"
win32:INCLUDEPATH += ../../marsyas "$$(DXSDK_DIR)"Include\ #include path for dsound.h

