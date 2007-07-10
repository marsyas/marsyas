#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarLpcWindow.h"


int main(int argc, char *argv[])
{

  QApplication app(argc, argv);


  MarLpcWindow* win = new MarLpcWindow();
  win->show();




  return app.exec();
}
