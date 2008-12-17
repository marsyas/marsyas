#include <QApplication>
#include <QtGui>

#include "MarSystem.h"
#include "MarNetworkCanvas.h"

/*
#include <QString>
#include <QFile>
void loadStyleSheet(const QString &sheetName)
{
	QFile file("" + sheetName + ".qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
//	std::cout << "\"" << styleSheet.toStdString() << "\"" << std::endl;

	qApp->setStyleSheet(styleSheet);
}
*/
int main(int argc, char *argv[])
{
	MarSystemManager mng;
	MarSystem* ser = mng.create("Series","ser");
	ser->addMarSystem(mng.create("AudioSource","src"));
	MarSystem* fan = mng.create("Fanout","fan");
	fan->addMarSystem(mng.create("Foo","foo"));
	fan->addMarSystem(mng.create("Bar","bar"));
	fan->addMarSystem(mng.create("Bop","bop"));
	ser->addMarSystem(fan);
	ser->addMarSystem(mng.create("Gain","gain"));
	ser->addMarSystem(mng.create("AudioSink","snk"));

//	Q_INIT_RESOURCE(stylesheet);

	QApplication app(argc, argv);
	MarNetworkCanvas zeus;
//	loadStyleSheet("style1");
	//zeus.loadStyleSheet("style1");
	zeus.setNetwork(ser);
	zeus.show();
	return app.exec();
}
