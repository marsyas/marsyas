/*
  At this point it is just a Tester file

  Author: Peter Minter
  Date Created: October 24 2005
*/
#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QtGui>
#include <iostream>
#include "MarCanvas.h"
#include "MarSystemNode.h"
#include "WidgetButton.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
  QApplication qapp(argc,argv);
  MainWindow w;
  // w.setGeometry(0,0,600,600);
  w.show();
  qapp.exec();
}
