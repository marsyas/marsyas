include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = Marx2DGraph 
INCLUDEPATH += . ..

# Input
HEADERS += Marx2DGraph.h TopPanelNew.h
SOURCES += main.cpp \
           Marx2DGraph.cpp \
           TopPanelNew.cpp
