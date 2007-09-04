include ( ../Qt4Apps.pri)

TEMPLATE = app
TARGET = MarWidgets
INCLUDEPATH += . ..
QT += opengl

unix:marsyasVorbis:LIBS += -lvorbis -lvorbisfile

# Input
HEADERS += MarWidgets.h MarControlWidget.h MarSpinBox.h MarLineEdit.h MarSlider.h 
SOURCES += main.cpp MarWidgets.cpp MarControlWidget.cpp MarSpinBox.cpp MarLineEdit.cpp MarSlider.cpp
