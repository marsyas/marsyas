#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarOscInWindow.h"
#include "CommandLineOptions.h"

#include "OscMapper.h"

void usage() {
  cout << "MarOscIn test.wav" << endl;
  cout << endl;
  cout << "MarOscIn is a tiny little Marsyas Qt based application that reads in" << endl;
  cout << "OSC messages that are sent to port 9000 to the OSC address" << endl;
  cout << "\"/gainSlider\" and changes the value of the QSlider \\gainSlider" << endl;
  cout << "based on the value in the contents of the OSC message." << endl;
  cout << endl;
  cout << "Please read the README file for more info" << endl;
  cout << endl;
  cout << "by sness - sness@sness.net (c) 2008" << endl;
}

int main(int argc, char **argv)
{

  if (argc < 2) {
	usage();
	exit(0);
  }

  QApplication* app = new QApplication(argc, argv);

  //
  // Create the main application window and pass it argv[1] as the
  // file to play to AudioSink.
  //
  MarOscInWindow* win = new MarOscInWindow(argv[1]);

  win->show();

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
  OscMapper* oscMapper = new OscMapper(inputOscHostAddress_, inputOscPort_, outputOscHostAddress_, outputOscPort_, 
									   app, win->getMarSystemQtWrapper ());

  //
  // Map any messages coming to the OSC port "/gainSlider" to the
  // win->gainSlider_ QSlider object
  //
  oscMapper->registerInputQtSlot (win->gainSlider_, "/gainSlider", QVariant::Int);

  return app->exec();
}
