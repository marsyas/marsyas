#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarOscMultiGainWindow.h"
#include "CommandLineOptions.h"

#include "OscMapper.h"

void usage() {
  cout << "MarOscMultiGain test1.wav test2.wav test3.wav" << endl;
  cout << endl;
  cout << "MarOscMultiGain is a tiny little Marsyas Qt based application that reads in" << endl;
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

  if (argc < 4) {
	usage();
	exit(0);
  }

  QApplication* app = new QApplication(argc, argv);

  //
  // Create the main application window and pass it argv[1] as the
  // file to play to AudioSink.
  //
  MarOscMultiGainWindow* win = new MarOscMultiGainWindow(argv[1],argv[2],argv[3]);

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
  oscMapper->registerInputQtSlot (win->gain1Slider_, "/key1", QVariant::Int);
  oscMapper->registerInputQtSlot (win->gain2Slider_, "/key2", QVariant::Int);
  oscMapper->registerInputQtSlot (win->gain3Slider_, "/key3", QVariant::Int);

  return app->exec();
}
