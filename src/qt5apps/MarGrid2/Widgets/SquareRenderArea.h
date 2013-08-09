#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <QColor>
#include <QSize>
#include <QLine>
#include <ostream>
#include <cstdlib>
#include <iostream>

// Color definitions
#ifndef GRID_COLOURS
#define PURPLE 160,32,240
#define BROWN 139,69,16
#define PINK 255,105,180
#define ORANGE 255,127,0
#endif /* GRID_COLOURS */

/***********************************************************

SquareRenderArea is an widget that draws the contents of a
given grid square with the size of the colour blocks proportional
to the percentrage of files in that square of the given genre.

************************************************************/

class SquareRenderArea : public QWidget
{
  Q_OBJECT

public:
  SquareRenderArea(QWidget *parent = 0);
  void updateSquare(int * genreDensity, int numGenres);

protected:
  void paintEvent(QPaintEvent *event);

private:
  QColor currentColour;
  int *squareDensity;
  double *squarePaintSize;
  int squareSize;
  int totalFiles;
};
