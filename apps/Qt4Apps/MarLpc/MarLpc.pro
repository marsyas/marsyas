include ( ../Qt4Apps.pri )

CONFIG += console

TEMPLATE = app
TARGET = MarLpc
INCLUDEPATH += . ..

# Input
HEADERS += \
	../MarSystemQtWrapper.h \
	MarLpcWindow.h \
	MarControlGUI.h 

SOURCES += \
	main.cpp \
    ../MarSystemQtWrapper.cpp \
	MarLpcWindow.cpp \
	MarControlGUI.cpp
