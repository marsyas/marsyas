include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = MarMonitors 
INCLUDEPATH += . ..

# Input
HEADERS += Marx2DGraph.h MarMonitors.h
SOURCES += main.cpp \
           Marx2DGraph.cpp \
           MarMonitors.cpp
