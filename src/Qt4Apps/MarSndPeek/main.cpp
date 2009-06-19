////////////////////////////////////////////////////////////////////////////////
//
// MarSndPeek
//
// An application that shows a 3D spectrogram in real-time using OpenGL.
//
// Inspired by sndpeek : http://soundlab.cs.princeton.edu/software/sndpeek/
//
// by sness (c) 2009 - GPL - sness@sness.net
//
////////////////////////////////////////////////////////////////////////////////

#include <QApplication>

#include "window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	string inAudioFileName;
	inAudioFileName = argv[1];
    Window window(inAudioFileName);
    window.show();
    return app.exec();
}
