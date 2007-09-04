include ( ../Qt4Apps.pri )

CONFIG += console

TEMPLATE = app
TARGET = MarLpc
INCLUDEPATH += . ..

QT += network

# Input
HEADERS += \
	../OscMapper.h \
	MarLpcWindow.h \
	MarControlGUI.h \
	../qoscclient.h \
	../qoscserver.h \
	../qosctypes.h

SOURCES += \
	main.cpp \
	../OscMapper.cpp \
	MarLpcWindow.cpp \
	MarControlGUI.cpp \
	../qoscclient.cpp \
	../qoscserver.cpp \
	../qosctypes.cpp 
