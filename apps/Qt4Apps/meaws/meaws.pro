SOURCES = start.cpp
HEADERS = mainwindow.h backend.h user.h metro.h exercise.h
SOURCES += mainwindow.cpp backend.cpp user.cpp metro.cpp exercise.cpp
HEADERS += ../MarSystemQtWrapper.h
SOURCES += ../MarSystemQtWrapper.cpp 
RESOURCES = icons.qrc
TARGET = meaws
VERSION = 0.4
#CONFIG = qt

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

