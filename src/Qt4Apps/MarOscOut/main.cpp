#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarOscOutWindow.h"
#include "CommandLineOptions.h"

#include "OscMapper.h"

void usage() {
  cout << "MarOscOut" << endl;
  cout << endl;
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
  MarOscOutWindow* win = new MarOscOutWindow(argv[1]);

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
									   app, win->getMarSystemQtWrapper());

  //
  // 
  // Map messages from the win->gainSlider_ QSlider object to output on that port
  //
  // The OSC address that it will send to is:
  //
  // ['/mrs_real/gain', ',f', 0.0]
  //
  oscMapper->registerOutputQtSlot (win->gainSlider_, "/gainSlider", QVariant::Int);

  return app->exec();
}
