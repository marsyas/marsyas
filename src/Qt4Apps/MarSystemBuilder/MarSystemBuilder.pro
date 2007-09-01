include ( ../Qt4Apps.pri ) 


TEMPLATE = app
TARGET = MarSystemBuilder
INCLUDEPATH += . ..

# Input
HEADERS += borderlayout.h \
           CanvasWidget.h \
           CompositeNode.h \
           DefaultCompositeNode.h \
           DefaultMarSystemNode.h \
           FanoutNode.h \
           GainNode.h \
           MainWindow.h \
           MarCanvas.h \
           MarSystemNode.h \
           MarSystemNodeFactory.h \
           SeriesNode.h \
           WidgetButton.h

SOURCES += borderlayout.cpp \
           CanvasWidget.cpp \
           CompositeNode.cpp \
           DefaultCompositeNode.cpp \
           DefaultMarSystemNode.cpp \
           FanoutNode.cpp \
           GainNode.cpp \
           main.cpp \
           MainWindow.cpp \
           MarCanvas.cpp \
           MarSystemNode.cpp \
           MarSystemNodeFactory.cpp \
           SeriesNode.cpp \
           WidgetButton.cpp
