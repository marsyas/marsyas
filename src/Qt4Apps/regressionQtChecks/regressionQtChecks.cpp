#include "MarBackend.h"
#include <QCoreApplication>
#include <QObject>

// usage: ./regressionQtChecks infile.wav
int main(int argc, char *argv[])
{
  string infile = argv[1];
  string outfile = "qtcheck.au";
  QCoreApplication app(argc, argv);

  MarBackend* marBackend = new MarBackend(infile, outfile);
  QObject::connect(marBackend, SIGNAL(quit()), &app, SLOT(quit()));

//	cout<<"begin wait"<<endl;
//	marBackend->waitUntilFinished();
//	sleep(1);
//	cout<<"done wait"<<endl;
  return app.exec();
}

