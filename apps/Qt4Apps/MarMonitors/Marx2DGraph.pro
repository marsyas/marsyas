include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = Marx2DGraph 
INCLUDEPATH += . ..

# Input
HEADERS += ../MarSystemQtWrapper.h Marx2DGraph.h TopPanelNew.h
SOURCES += main.cpp \
           ../MarSystemQtWrapper.cpp \
           Marx2DGraph.cpp \
           TopPanelNew.cpp
