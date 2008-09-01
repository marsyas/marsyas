###  your files
SOURCES = main.cpp
HEADERS = mainwindow.h
SOURCES += mainwindow.cpp
HEADERS += backend.h
SOURCES += backend.cpp


### if running inside the source tree
MARSYAS_INSTALL_DIR = ../../../
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyasqt/
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyas/
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/release/


### if installed elsewhere
#MARSYAS_INSTALL_DIR = /usr/local
#MARSYAS_INSTALL_DIR = ${HOME}/usr/
#INCLUDEPATH += $$MARSYAS_INSTALL_DIR/marsyas/
#LIBPATH += $$MARSYAS_INSTALL_DIR/lib/


### basic OS stuff; do not change!
win32-msvc2005:LIBS += marsyas.lib marsyasqt.lib
unix:LIBS += -lmarsyas -lmarsyasqt -L$$MARSYAS_INSTALL_DIR/lib
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

