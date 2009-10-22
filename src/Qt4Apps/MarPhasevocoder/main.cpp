#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarPhasevocoderWindow.h"
#include "OscMapper.h" 

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  string inAudioFileName = "";
  if (argc > 1) {
	inAudioFileName = argv[1];	  
  }
  cout << inAudioFileName << endl;
  

  MarPhasevocoderWindow* win = new MarPhasevocoderWindow(inAudioFileName);
  win->show();

  OscMapper* oscMapper;


  return app.exec();
}
