#include "MarBackend.h"
#include <QApplication> 

// usage: ./regressionQtChecks infile.wav
int main(int argc, char *argv[])
{
	string infile = argv[1];
	string outfile = "qtcheck.au";
	QApplication app(argc, argv);

	MarBackend* marBackend = new MarBackend(infile, outfile);

	cout<<"begin wait"<<endl;
	marBackend->waitUntilFinished();
	cout<<"done wait"<<endl;
	return app.exec();
}

