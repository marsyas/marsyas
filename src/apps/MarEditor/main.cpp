#include <QtGui/QApplication>
#include "MarEditor.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MarEditor w;
  w.show();
  a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
  return a.exec();
}
