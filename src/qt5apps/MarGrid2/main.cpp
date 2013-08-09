/*
** Copyright (C) 2000-2013 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <QApplication>

#include "MainWindow.h"
//#include "OscMapper.h"

int main( int argc, char *argv[] ) {
  QApplication app(argc, argv);
  Grid* grid = new Grid();
  MainWindow mainWin(grid);
  mainWin.show();
  grid->start();

  //
  // Map OSC messages to change songs on the grid
  //
#if 0
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
#endif

  return app.exec();
}

