////////////////////////////////////////////////////////////////////////////////
//
// MarPanning
//
// An application that shows the left/right panning of an audio source as a
// graphical OpenGL display
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
  cout << "MarPanning : Display the left/right panning of an audio source as a" << endl;
  cout << "graphical OpenGL display" << endl;
  cout << endl;
  cout << "usage:" << endl;
  cout << "MarPanning infile.wav" << endl;
  cout << endl;
  cout << endl;
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  string inAudioFileName = "";
  if (argc > 1) {
	inAudioFileName = argv[1];	  
  }
  Window window(inAudioFileName);
  window.show();
  return app.exec();
}
