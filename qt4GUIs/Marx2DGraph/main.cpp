#include <QApplication>
#include <math.h>
#include <iostream>

#include "Marx2DGraph.h"
#include <QPushButton>
#include "TopPanel.h"
#include "TopPanelNew.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  // TopPanel panel;
  TopPanelNew panel;
  
  panel.setGeometry(100, 100, 800, 600);
  panel.show();

  return app.exec();
}
