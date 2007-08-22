#  your files
SOURCES = main.cpp
HEADERS = mainwindow.h
SOURCES += mainwindow.cpp
HEADERS += backend.h
SOURCES += backend.cpp


MARSYAS_INSTALL_DIR = ../../
#MARSYAS_INSTALL_DIR = ${HOME}/usr/
#MARSYAS_INSTALL_DIR = /usr/local
message("Marsyas was installed in $$MARSYAS_INSTALL_DIR, right?")


INCLUDEPATH += $$MARSYAS_INSTALL_DIR/marsyas/
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/release

unix:LIBS += -lmarsyas -lmarqtlib -L$$MARSYAS_INSTALL_DIR/lib
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

