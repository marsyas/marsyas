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
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "window.h"

void usage()
{
  cout << "MarSndPeek : Display a realtime spectrum waterfall and waveform of an audio file" << endl;
  cout << endl;
  cout << "usage:" << endl;
  cout << "MarSndPeek infile.wav" << endl;
  cout << endl;
  cout << endl;
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();

  QString inAudioFileName;
  if (arguments.count() > 1)
    inAudioFileName = arguments[1];

  Window window(inAudioFileName);
  window.show();

  return app.exec();
}
