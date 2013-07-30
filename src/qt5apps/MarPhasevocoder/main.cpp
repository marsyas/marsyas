#include "MarPhasevocoderWindow.h"

#include <QApplication>

using namespace std;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QStringList args = app.arguments();

  QString inAudioFileName;
  if (args.count() > 1)
    inAudioFileName = args[1];

  MarPhasevocoderWindow* win = new MarPhasevocoderWindow(inAudioFileName);
  win->show();

  return app.exec();
}
