#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "Marx2DGraph.h"
#include "MarMonitors.h"

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  if (argc < 2) {
	std::cout << "Usuage:\n\t$ ./Marx2DGraph /path/to/audiofile.au\n";
    return 0;
  }

  std::string progName = argv[0];
  std::string audioFile = argv[1];

  MarMonitors* panel = new MarMonitors( audioFile );
  
  panel->setGeometry(50, 50, 800, 500);
  panel->show();

  return app.exec();
}
