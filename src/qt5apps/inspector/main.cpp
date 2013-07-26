#include "main_controller.h"

#include <MarSystemManager.h>

#include <QDebug>
#include <QApplication>
#include <QGuiApplication>
#include <QStringList>

#include <fstream>

using namespace std;
using namespace Marsyas;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList arguments = app.arguments();
    if (arguments.size() < 2) {
      qWarning("Missing argument: marsyas plugin file.");
      return 1;
    }

    QString plugin_filename = arguments[1];
    ifstream plugin_stream( plugin_filename.toStdString().c_str() );
    MarSystemManager mng;
    MarSystem *system = mng.getMarSystem(plugin_stream);
    if (!system) {
      qCritical("Could not load plugin file!");
      return 1;
    }

    Main *main_controller = Main::create(system);

    return app.exec();
}
