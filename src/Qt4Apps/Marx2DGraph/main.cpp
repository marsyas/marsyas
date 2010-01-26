#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "Marx2DGraph.h"
#include "TopPanel.h"

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  std::string progName = argv[0];

  TopPanel* panel = new TopPanel();
  
  panel->setGeometry(50, 50, 800, 500);
  panel->show();

  return app.exec();
}
