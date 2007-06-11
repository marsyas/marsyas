#  your files
SOURCES = main.cpp
HEADERS = mainwindow.h
SOURCES += mainwindow.cpp
HEADERS += backend.h
SOURCES += backend.cpp

#  these files are common to every Marsyas/QT app 
HEADERS += MarSystemQtWrapper.h
SOURCES += MarSystemQtWrapper.cpp

#  basic system variables
TARGET = runme
INCLUDEPATH += ${HOME}/usr/include/marsyas
unix:LIBS += -lmarsyas -L/usr/local/lib # -lmad -lvorbis -lvorbisfile
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

