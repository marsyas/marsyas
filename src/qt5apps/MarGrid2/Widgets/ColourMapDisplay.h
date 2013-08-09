#ifndef COLOURMAP_H
#define COLOURMAP_H

// Color definitions
#ifndef GRID_COLOURS
#define PURPLE 160,32,240
#define BROWN 139,69,16
#define PINK 255,105,180
#define ORANGE 255,127,0
#endif /* GRID_COLOURS */

#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <QColor>
#include <QGroupBox>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "SquareRenderArea.h"


/***********************************************************

ColourMapDisplay is an widget containing the widgets
pertaining to the display of the genres of a grid square.


************************************************************/

class ColourMapDisplay : public QWidget
{
  Q_OBJECT

public:
  ColourMapDisplay(QWidget *parent = 0);
  ~ColourMapDisplay();

public slots:
  void updateSquare(int * genreDensity, int numGenres);

private:
  int startPos;
  int * genreDensity;
  int numGenres;
  SquareRenderArea *renderArea;
  QGroupBox *colourKey;
  QLabel classical;
  QLabel blues;
  QLabel country;
  QLabel disco;
  QLabel hiphop;
  QLabel jazz;
  QLabel metal;
  QLabel reggae;
  QLabel rock;
  QLabel pop;

};
#endif /* COLOURMAP_H */