include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = MarGrid
INCLUDEPATH += . ..

unix:marsyasVorbis:LIBS += -lvorbis -lvorbisfile 

HEADERS += ColorMap.h SpectraColormap.h GreyScaleColormap.h MarGrid.h MarSystemWrapper.h MarGridWindow.h  
SOURCES += main.cpp ColorMap.cpp SpectraColormap.cpp GreyScaleColormap.cpp MarGrid.cpp MarSystemWrapper.cpp MarGridWindow.cpp 
