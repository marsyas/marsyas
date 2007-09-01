#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarPhasevocoderWindow.h"


int main(int argc, char *argv[])
{

  QApplication app(argc, argv);


  MarPhasevocoderWindow* win = new MarPhasevocoderWindow();
  win->show();




  return app.exec();
}
