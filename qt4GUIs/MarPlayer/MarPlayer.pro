TEMPLATE = app
VERSION = 0.0.0
TARGET += 
DEPENDPATH += .
INCLUDEPATH += ../../marsyas
unix:LIBS += -lmarsyas -L../../marsyas -lmad 
unix:!macx:LIBS += -lasound -lvorbis -lvorbisfile 
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation
DISTFILES     += README
RESOURCES     += MarPlayerForm.qrc
ICON = MarPlayer.icns
FORMS 	      += MarPlayerForm.ui
HEADERS       += MarPlayer.h ../MarSystemQtWrapper.h Mapper.h
SOURCES       += main.cpp MarPlayer.cpp ../MarSystemQtWrapper.cpp Mapper.cpp


win32:DEFINES += __WINDOWS_DS__
win32:LIBS += -lmarsyas  -ldsound  
win32:LIBPATH += ..\..\marsyasVisualStudio2005\marsyas\Release "$$(DXSDK_DIR)Lib\x86\"
win32:INCLUDEPATH += ../../marsyas "$$(DXSDK_DIR)"Include\ #include path for dsound.h
