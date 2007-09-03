include ( ../Qt4Apps.pri )

TEMPLATE = app
TARGET = MarPhasevocoder
#INCLUDEPATH += . ..
INCLUDEPATH += $$BASEDIR/src/marsyasqt_wrapper/

# Input
HEADERS += \
#	../MarSystemQtWrapper.h \
	MarPhasevocoderWindow.h \
	MarControlGUI.h 

SOURCES += \
	main.cpp \
#    ../MarSystemQtWrapper.cpp \
	MarPhasevocoderWindow.cpp \
	MarControlGUI.cpp
