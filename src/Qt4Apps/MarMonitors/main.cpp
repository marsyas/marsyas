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

  string pluginName = "";
  string audioInputName = "";
  if (argc > 1) 
    {
      pluginName = argv[1];
    }
  if (argc > 2) 
    {
      audioInputName = argv[2];
    }

  std::string audioFile = "/home/gtzan/data/sound/music_speech/music/gravity.au";

  MarMonitors* monitors = new MarMonitors(pluginName, audioInputName);
  
  monitors->setGeometry(50, 50, 1400, 500);
  monitors->show();

  return app.exec();
}
