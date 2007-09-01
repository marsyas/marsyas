#include <QApplication>
#include <QPushButton>

#include <cmath>
#include <iostream>

#include "MarxGLColorGraph.h"
#include "TopPanelNew.h"

using namespace std;

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  if (argc < 2) {
    cout << "Usuage:\n\t$ ./Marx2DGraph /path/to/audiofile.au\n";
    return 0;
  }

  string progName = argv[0];
  string audioFile = argv[1];

  TopPanelNew* panel = new TopPanelNew( audioFile );
  
  panel->setGeometry(100, 100, 600, 196);
  panel->show();

  return app.exec();
}
