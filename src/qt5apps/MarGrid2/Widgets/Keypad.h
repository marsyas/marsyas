#ifndef KEYPAD_H
#define KEYPAD_H

#include <ostream>

#include <QList>
#include <QPoint>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>

#include <ctime>
#include <cstdlib>

#include "../Interface/MyDisplay.h"
#include "../Interface/Classifier.h"
#include "../Interface/Extractor.h"

#include "../Music/MusicCollection.h"
//#include "../Marsyas/MarsyasECP.h"
#include "../Colormaps/Colormap.h"
#include "Tracklist.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QPainter;

class Classifier;
class Extractor;
class MarsyasECP;
class KeypadSquare;

const static int KEYPAD_SIZE = 9;
enum KeypadEnum  { KEYPAD1, KEYPAD2, KEYPAD3,
                   KEYPAD4, KEYPAD5, KEYPAD6,
                   KEYPAD7, KEYPAD8, KEYPAD9, KEYPAD_INVALID
                 };

class Keypad : public MyDisplay
{
  Q_OBJECT

public:
  Keypad(int winSize, Tracklist *tracklist, QWidget *parent = 0);
  ~Keypad();
  void clear();

  int getHeight() const { return _width; }
  int getWidth() const { return _width; }

public slots:
  void extract() {}
  void predict() {}
  void train() {}
  void midiXYEvent(unsigned char xaxis, unsigned char yaxis);
  void midiPlaylistEvent(bool next);
  void keypadClicked(KeypadEnum button);

protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  void startDrag(Qt::DropActions supportedActions);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

  void updateXYPosition(int x, int y);
  void paintEvent(QPaintEvent *event);
  void playNextTrack();

  void addTrack(int x, int y, MusicTrack* track);
  void resetGrid();

  void setGridX(int x);
  void setGridY(int y);
  KeypadSquare *getCurrentSquare();

  void setup();

private:
  int _winSize;
  int _cellSize;
  int _gridX;
  int _gridY;
  int _width;

  KeypadEnum _currentIndex;
  KeypadSquare *_current;
  KeypadSquare *_root;

  //MarsyasECP *_marsyas;
  //Classifier *_classifier;
  //Extractor *_extractor;

  MusicCollection *_collection;
};

/*
Keypad Arrangement

PAD1	PAD2	PAD3

PAD4	PAD5	PAD6

PAD7	PAD8	PAD9
*/
class KeypadSquare
{
public:
  KeypadSquare(KeypadSquare *parent=NULL);
  ~KeypadSquare();

  bool isEmpty() const;

  void addTrack(MusicTrack *track);
  void removeTrack(MusicTrack *track);

  void nextTrack();
  MusicTrack* getCurrent();
  MusicTrackIterator getTracks();

  int getCount() const;

  void clear();

  KeypadSquare *getParent();
  KeypadSquare *getChild(KeypadEnum child);
  KeypadSquare *createChild(KeypadEnum child);
  KeypadEnum getChildId(KeypadSquare *child);

private:
  KeypadSquare *_children[KEYPAD_SIZE];
  KeypadSquare *_parent;

  MusicTrackVector _list;
  int _currentTrack;
};

#endif
