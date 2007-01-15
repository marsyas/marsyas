TEMPLATE = app
CONFIG += qt release
TARGET += 
DEPENDPATH += .

win32:INCLUDEPATH += ../../marsyas
unix:INCLUDEPATH += /usr/local/include/marsyas

win32:LIBS += -lmarsyas -L..\..\marsyasVisualStudio2005\marsyas\Release 
win32:LIBS += -luser32
win32:LIBS += -ldsound -L\"C:\Program Files\Microsoft DirectX SDK (February 2006)\Lib\x86\"

unix:LIBS += -lmarsyas -L/usr/local/lib -lmad -lvorbis -lvorbisfile
unix:LIBS += -lasound
unix:LIBS += -lm

macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

# Input
HEADERS += MarSystemWrapper.h MarPhasevocoderWindow.h  
SOURCES += main.cpp \
           MarSystemWrapper.cpp \
	         MarPhasevocoderWindow.cpp 
