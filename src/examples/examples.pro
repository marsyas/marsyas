#  your main file.  (uncomment **ONE** of these lines)
#SOURCES = helloworld.cpp
SOURCES = gettingdata.cpp
#SOURCES = controls.cpp
#SOURCES = commandOptions.cpp
#SOURCES = dataflow-split.cpp
#SOURCES = gettingdata.cpp
#SOURCES = writefile.cpp

# add as necessary
#HEADERS += extra_file.h
#SOURCES += extra_file.cpp


TARGET = my_program

### if running inside the source tree
MARSYAS_INSTALL_DIR = ../../
INCLUDEPATH += $$MARSYAS_INSTALL_DIR/src/marsyas/
LIBPATH += $$MARSYAS_INSTALL_DIR/lib/release/


### if installed elsewhere
#MARSYAS_INSTALL_DIR = /usr/local
#MARSYAS_INSTALL_DIR = ${HOME}/usr/
#INCLUDEPATH += $$MARSYAS_INSTALL_DIR/marsyas/
#LIBPATH += $$MARSYAS_INSTALL_DIR/lib/


### basic OS stuff; do not change!
win32-msvc2005:LIBS += marsyas.lib
win32:LIBS += -lpthread -ldsound -lwinmm
unix:LIBS += -lmarsyas -L$$MARSYAS_INSTALL_DIR/lib
!macx:LIBS += -lasound
macx:LIBS += -framework CoreAudio -framework CoreMidi -framework CoreFoundation

