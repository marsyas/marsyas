TEMPLATE = app
VERSION = 0.0.0
TARGET += 
DEPENDPATH += .
INCLUDEPATH += /usr/local/include/marsyas
unix:LIBS += -lmarsyas -L/usr/local/lib  
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation
DISTFILES     += README
RESOURCES     += MarPlayerForm.qrc
ICON = MarPlayer.icns
FORMS 	      += MarPlayerForm.ui
HEADERS       += MarPlayer.h MarSystemWrapper.h Mapper.h
SOURCES       += main.cpp MarPlayer.cpp MarSystemWrapper.cpp Mapper.cpp

