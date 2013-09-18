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
//#include "OscMapper.h"
//using namespace MarsyasQt;

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

  QStringList args = app.arguments();
  QString fileName;
  if (args.size() > 1)
    fileName = args[1];

  Window window;
  window.show();
  window.play(fileName);

#if 0
  //
  // Setup the input and output OSC host addresses and ports
  //
  QHostAddress inputOscHostAddress_ = QHostAddress::LocalHost;
  QHostAddress outputOscHostAddress_ = QHostAddress::LocalHost;
  mrs_natural inputOscPort_ = 9000;
  mrs_natural outputOscPort_ = 9001;

  //
  // Create an OscMapper that maps messages to the app through a
  // MarSystemQtWrapper.
  //
  OscMapper* oscMapper = new OscMapper(inputOscHostAddress_, inputOscPort_,
                                       outputOscHostAddress_, outputOscPort_,
                                       app, window.getMarSystemQtWrapper ());

  /* oscMapper->registerInputQtSlot(window.xTransSlider, "/key1", QVariant::Int);
  oscMapper->registerInputQtSlot(window.xTransSlider, "/key2", QVariant::Int);
  oscMapper->registerInputQtSlot(window.xTransSlider, "/key3", QVariant::Int);
  */

  oscMapper->registerInputQtSlot(window.magnitudeCutoffSlider, "/keym", QVariant::Int);
  oscMapper->registerInputQtSlot(window.numVerticesSlider, "/keys", QVariant::Int);
  oscMapper->registerInputQtSlot(window.dotSizeSlider, "/keyd", QVariant::Int);
  oscMapper->registerInputQtSlot(window.yRotSlider, "/keyleft", QVariant::Int);
  oscMapper->registerInputQtSlot(window.yRotSlider, "/keyright", QVariant::Int);
  oscMapper->registerInputQtSlot(window.xRotSlider, "/keyup", QVariant::Int);
  oscMapper->registerInputQtSlot(window.xRotSlider, "/keydown", QVariant::Int);
#endif
  return app.exec();
}
