#ifndef GRIDDISPLAY_H
#define GRIDDISPLAY_H


#include <ostream>

#include <QList>
#include <QPoint>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QWaitCondition>
#include <QTimer>
#include <QCursor>
#include <QDesktopWidget>
#include <QApplication>
#include <QToolTip>

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Collection.h>

#include "../Music/MusicCollection.h"
#include "../Colormaps/Colormap.h"

#include "../Widgets/Tracklist.h"
#include "../Widgets/Grid.h"

// Color definitions
#ifndef GRID_COLOURS
#define PURPLE 160,32,240
#define BROWN 139,69,16
#define PINK 255,105,180
#define ORANGE 255,127,0
#endif /* GRID_COLOURS */

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QPainter;

class Classifier;
class Extractor;
class MarsyasECP;
class GridSquare;

using namespace MarsyasQt;

class GridDisplay : public MyDisplay
{
  Q_OBJECT
public:
  GridDisplay(int winSize, Tracklist *tracklist, Grid* grid_, QWidget *parent);
  ~GridDisplay();
  void clear();

  int getHeight() const { return grid_->getHeight(); }
  int getWidth() const { return grid_->getWidth(); }

  void keyMove(QKeyEvent *keyEvent);


public slots:
  void midiXYEvent(unsigned char xaxis, unsigned char yaxis);
  void midiPlaylistEvent(bool next);
  void reload();
  void extract();
  void predict();
  void train();
  void init();
  void openPredictionGrid(QString fname);
  void savePredictionGrid(QString fname);
  void playModeChanged();
  void cancelButton();
  void repaintSlot();
  void hashLoad();
  void fullScreenMouse();
  void fullScreenMouseMove();
  void colourMapMode();
  void playlistSelected(QString playlist);
  void setXGridSize(QString);
  void setYGridSize(QString);
  void showErrorMessage(QString);
  void resetGrid();

signals:
  void extractMode();
  void trainMode();
  void predictMode();
  void initMode();
  void clearMode();
  void openPredictionGridSignal(QString fname);
  void savePredictionGridSignal(QString fname);
  void cancelButtonPressed();
  void hashLoadPressed();
  void fullScreenMode(bool modeOn);
  void updateColourMap(int *, int);
  void resetGridAction();

protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  void startDrag(Qt::DropActions supportedActions);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  bool event(QEvent *event);

  void updateXYPosition(int x, int y);
  void updateXY1Position(int x, int y);
  void paintEvent(QPaintEvent *event);
  void playNextTrack();

private:
  Grid* grid_;

  QList<QPixmap> piecePixmaps;
  QList<QRect> pieceRects;
  QList<QPoint> pieceLocations;
  QVector<bool> squareHasInitialized;
  QWaitCondition buttonPressed;
  QTimer *fullScreenTimer;
  QCursor *mouseCursor;
  QPoint lastMousePoint;


  QRect highlightedRect;
  QRect metalRec;
  QRect classicalRec;
  int inPlace;
  int _winSize;
  int _cellSize;
  int oldXPos;
  int oldYPos;
  int oldX1Pos;
  int oldY1Pos;
  int lastIndex;
  bool fullScreenMouseOn;
  bool initDone;
  bool colourMapMode_;

};

#endif
