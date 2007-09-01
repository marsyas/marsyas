#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarPeaks.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  MarPeaks* win = new MarPeaks();
  win->show();

  return app.exec();
}
