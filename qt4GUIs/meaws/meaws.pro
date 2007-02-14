SOURCES = start.cpp
HEADERS = mainwindow.h backend.h user.h metro.h
SOURCES += mainwindow.cpp backend.cpp user.cpp metro.cpp
HEADERS += ../MarSystemQtWrapper.h
SOURCES += ../MarSystemQtWrapper.cpp 
RESOURCES = icons.qrc
TARGET = runme
VERSION = 0.4
#CONFIG = qt
INCLUDEPATH += ${HOME}/usr/include/marsyas
unix:LIBS += -lmarsyas -L${HOME}/usr/lib 
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

