#  your files
SOURCES = main.cpp
HEADERS = mainwindow.h
SOURCES += mainwindow.cpp
HEADERS += backend.h
SOURCES += backend.cpp

#  these files are common to every Marsyas/QT app 
#  you will need to update these paths (and the paths in backend.h)
#  if you copy this directory elsewhere.
HEADERS += ../../apps/Qt4Apps/MarSystemQtWrapper.h
SOURCES += ../../apps/Qt4Apps/MarSystemQtWrapper.cpp

#MARSYAS_INSTALL_DIR  = ${HOME}/usr/
#MARSYAS_INSTALL_DIR  = /usr/local
message("Marsyas was installed in $$MARSYAS_INSTALL_DIR, right?")

INCLUDEPATH += $$MARSYAS_INSTALL_DIR/include/marsyas
INCLUDEPATH += ../../bin/release
unix:LIBS += -lmarsyas -L$$MARSYAS_INSTALL_DIR/lib # -lmad -lvorbis -lvorbisfile
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

