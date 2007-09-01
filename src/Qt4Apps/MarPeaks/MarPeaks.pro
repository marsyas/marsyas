include ( ../Qt4Apps.pri )

TEMPLATE = app
TARGET += MarPeaks
INCLUDEPATH += . ..

# Qwt stuff
!exists("$$BASEDIR"/otherlibs/Qwt/lib){
	warning("Qwt lib not yet built! Build Qwt first!")
}
INCLUDEPATH += "$$BASEDIR"/otherlibs/Qwt/src
CONFIG(release, debug|release){
	LIBS += -lqwt -L\"$$BASEDIR/otherlibs/Qwt/lib/release\"
}
CONFIG(debug, debug|release){
	LIBS += -lqwt -L\"$$BASEDIR/otherlibs/Qwt/lib/debug\"
}

     LIBPATH+= /usr/local/lib
     LIBS += -lQGLViewer
     DEFINES += MARSYAS_QGLVIEWER 
     INCLUDEPATH += /usr/local/include/qglviewer



# Input
HEADERS += \
	../MarSystemQtWrapper.h \
	MarPeaks.h \
	SpectrogramData.h \
	plot.h

SOURCES += \
	main.cpp \
    ../MarSystemQtWrapper.cpp \
	MarPeaks.cpp \
	SpectrogramData.cpp \
	plot.cpp
