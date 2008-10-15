#include <QApplication>

#include "MainWindow.h"

int main( int argc, char *argv[] ) {
	QApplication app(argc, argv);
	Grid* grid = new Grid();
	MainWindow mainWin(grid);
	mainWin.show();
	grid->start();
	return app.exec();
}

