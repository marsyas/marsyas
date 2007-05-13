#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {

	// causes linking troubles in MSVC
	//Q_INIT_RESOURCE(icons);

	QApplication app(argc, argv);
	MainWindow mainWin;
	mainWin.show();
	return app.exec();
}

