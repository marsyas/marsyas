include ( ../Qt4Apps.pri )

TEMPLATE = app
TARGET = MarPhasevocoder

# Input
HEADERS += \
	MarPhasevocoderWindow.h \
	MarControlGUI.h 

SOURCES += \
	main.cpp \
	MarPhasevocoderWindow.cpp \
	MarControlGUI.cpp
