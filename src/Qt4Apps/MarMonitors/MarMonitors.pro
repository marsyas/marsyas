include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = MarMonitors 
INCLUDEPATH += . ..

# Input
HEADERS += ../MarSystemQtWrapper.h Marx2DGraph.h MarMonitors.h
SOURCES += main.cpp \
           ../MarSystemQtWrapper.cpp \
           Marx2DGraph.cpp \
           MarMonitors.cpp
