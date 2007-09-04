include ( ../Qt4Apps.pri )

TEMPLATE = app
TARGET = MarPlayer
INCLUDEPATH += . ..

unix:marsyasVorbis:LIBS += -lvorbis -lvorbisfile 
FORMS 	      = MarPlayerForm.ui
DISTFILES     += README
RESOURCES     = MarPlayerForm.qrc
ICON 		  = MarPlayer.icns
HEADERS       += MarPlayer.h Mapper.h
SOURCES       += main.cpp MarPlayer.cpp Mapper.cpp

