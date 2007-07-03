include (../Qt4Apps.pri)

SOURCES = start.cpp mainwindow.cpp
HEADERS = defs.h mainwindow.h

HEADERS += user.h exerciseDispatcher.h
SOURCES += user.cpp exerciseDispatcher.cpp

HEADERS += exerciseAbstract.h exerciseIntonation.h exerciseControl.h
SOURCES += exerciseAbstract.cpp exerciseIntonation.cpp exerciseControl.cpp

HEADERS += backend.h
SOURCES += backend.cpp

#temporary
HEADERS += ../QtMarPlot.h
SOURCES += ../QtMarPlot.cpp

HEADERS += ../MarSystemQtWrapper.h
SOURCES += ../MarSystemQtWrapper.cpp

#HEADERS += backend.h metro.h
#SOURCES += mainwindow.cpp backend.cpp metro.cpp
#HEADERS += user.h exercise.h analyze.h display.h
#SOURCES += user.cpp exercise.cpp analyze.cpp display.cpp
#HEADERS += ../MarSystemQtWrapper.h ../QtMarPlot.h
#SOURCES += ../MarSystemQtWrapper.cpp ../QtMarPlot.cpp

RESOURCES = icons.qrc
TARGET = meaws
VERSION = 0.5
#CONFIG = qt

##  You might want to change this:
unix:HOME = /home/gperciva
macx: HOME = /Users/gperciva
win32:MEAWS_DATADIR = c:\temp\meaws
unix:MEAWS_DATADIR = $${HOME}/.meaws/

message("Installing data to $$MEAWS_DATADIR")
data.path = $$MEAWS_DATADIR
data.files = data/
INSTALLS += data


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

