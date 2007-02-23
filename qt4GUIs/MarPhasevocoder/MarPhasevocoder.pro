TEMPLATE = app
CONFIG += qt release
TARGET += 
DEPENDPATH += .

#CONFIG(debug) {
#  message(Building with debug support.)
#  DEFINES += MRSDEBUGGING
#}

#CONFIG(release) {
#  message(Building with release support.)
#}

win32:INCLUDEPATH += ../../marsyas
win32:LIBS += -lmarsyas -L..\..\marsyasVisualStudio2005\marsyas\Release 
win32:LIBS += -luser32
#win32:LIBS += -ldsound -L\"C:\Program Files\Microsoft DirectX SDK (February 2006)\Lib\x86\"
win32:LIBS += -ldsound -L\"$$(DXSDK_DIR)Lib\x86\"

unix:INCLUDEPATH += ../../marsyas
unix:LIBS += -lmarsyas -L../../marsyas -lmad
unix:LIBS += -lasound
unix:LIBS += -lm

macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

# Input
HEADERS += ../MarSystemQtWrapper.h MarPhasevocoderWindow.h MarControlGUI.h 
SOURCES += main.cpp \
           ../MarSystemQtWrapper.cpp \
	       MarPhasevocoderWindow.cpp \
	       MarControlGUI.cpp
