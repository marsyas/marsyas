#include <QApplication>

#include "MainWindow.h"
#include "OscMapper.h"

int main( int argc, char *argv[] ) {
	QApplication app(argc, argv);
	Grid* grid = new Grid();
	MainWindow mainWin(grid);
	mainWin.show();
	grid->start();

	//
	// Map OSC messages to change songs on the grid
	//
	
	// Get messages from localhost by default
	QHostAddress inputOscHostAddress_ = QHostAddress::LocalHost;
	QHostAddress outputOscHostAddress_ = QHostAddress::LocalHost;

	// Input OSC port is 9005 and output port is 9006
    quint16 inputOscPort_ = 9005;
	quint16 outputOscPort_ = 9006;

	OscMapper* oscMapper = new OscMapper(inputOscHostAddress_, inputOscPort_, outputOscHostAddress_, outputOscPort_, &app, grid->getMarSystemQtWrapper());

	// When a message is sent to the /update slot, update the two grid
	// pointers to their new locations
	oscMapper->registerInputQtSlot (grid, "/update", QVariant::Int);

	// The x and y locations of the first pointer
	oscMapper->registerInputQtSlot (grid->x0_, "/x0", QVariant::Int);
	oscMapper->registerInputQtSlot (grid->y0_, "/y0", QVariant::Int);

	// The x and y locations of the second pointer
	oscMapper->registerInputQtSlot (grid->x1_, "/x1", QVariant::Int);
	oscMapper->registerInputQtSlot (grid->y1_, "/y1", QVariant::Int);


	return app.exec();
}

