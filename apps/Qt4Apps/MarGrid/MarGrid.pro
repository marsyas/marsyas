include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = MarGrid
INCLUDEPATH += . ..

unix:marsyasVorbis:LIBS += -lvorbis -lvorbisfile 


HEADERS += MarGrid.h MarSystemWrapper.h MarGridWindow.h
SOURCES += main.cpp MarGrid.cpp MarSystemWrapper.cpp MarGridWindow.cpp
