include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = MarGrid
INCLUDEPATH += . ..


HEADERS += ColorMap.h SpectraColormap.h GreyScaleColormap.h MarGrid.h MarGridWindow.h  
SOURCES += main.cpp ColorMap.cpp SpectraColormap.cpp GreyScaleColormap.cpp MarGrid.cpp MarGridWindow.cpp 
