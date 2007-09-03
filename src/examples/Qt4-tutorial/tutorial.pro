#  your files
SOURCES = main.cpp
HEADERS = mainwindow.h
SOURCES += mainwindow.cpp
HEADERS += backend.h
SOURCES += backend.cpp


MARSYAS_INSTALL_DIR = ../../../  # works in the svn tree
#MARSYAS_INSTALL_DIR = /usr/local
#MARSYAS_INSTALL_DIR = ${HOME}/usr/
message("Hopefully, Marsyas was installed in $$MARSYAS_INSTALL_DIR")


INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyas/
#INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyas/marsyasqt/
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyasqt_wrapper/  # in svn
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/release/    # in the svn tree

unix:LIBS += -lmarsyas -lmarsyasqt

unix:!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

