###  your files
SOURCES = regressionQtChecks.cpp
HEADERS += MarBackend.h
SOURCES += MarBackend.cpp

#HEADERS += Waiter.h
#SOURCES += Waiter.cpp
#HEADERS += global.h


### if running inside the source tree
MARSYAS_INSTALL_DIR = ../../../
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyas/
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyasqt/
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/release/

### if installed elsewhere
#MARSYAS_INSTALL_DIR = /usr/local
#MARSYAS_INSTALL_DIR = ${HOME}/usr/
#INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyas/
#LIBPATH += $$MARSYAS_INSTALL_DIR/lib/

unix:LIBS += -lmarsyas -lmarsyasqt
unix:!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

