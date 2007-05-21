include ( ../Qt4Apps.pri )

TEMPLATE = app
TARGET = MarPlayer
INCLUDEPATH += . ..

unix:marsyasVorbis:LIBS += -lvorbis -lvorbisfile 

DISTFILES     += README
RESOURCES     = MarPlayerForm.qrc
ICON 		   = MarPlayer.icns
HEADERS       += MarPlayer.h ../MarSystemQtWrapper.h Mapper.h
SOURCES       += main.cpp MarPlayer.cpp ../MarSystemQtWrapper.cpp Mapper.cpp
FORMS 	      = MarPlayerForm.ui

