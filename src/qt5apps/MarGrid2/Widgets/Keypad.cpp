#include <QtGui>
#include "Keypad.h"

Keypad::Keypad(int winSize, Tracklist *tracklist, QWidget *parent)
  : MyDisplay(tracklist, parent), _winSize(winSize)
{
  _collection = MusicCollection::getInstance();
  _width = 3;
  _gridX = 0;
  _gridY = 0;
  setAcceptDrops(true);
  setMouseTracking(true);
  setMinimumSize(winSize, winSize);

  setup();
}

Keypad::~Keypad() {
  delete _root;
}

void Keypad::clear() {
  delete _root;
  setup();
}

void Keypad::setup() {
  qDebug("Setting up keypad\n");
  _cellSize = _winSize/_width;
  _root = new KeypadSquare(NULL);
  _current = _root;
}

void Keypad::updateXYPosition(int x, int y) {
  if ( 0 <= x && x <= _winSize && 0 <= y && y <= _winSize ) {
    setGridX(x);
    setGridY(y);

    for(int i=0; i < _width; i++) {
      for(int j=0; i < _width; j++) {
        if ( i*_cellSize <= x && x <= (i+1)*_cellSize &&
             j*_cellSize <= y && y <= (j+1)*_cellSize ) {
          _currentIndex = (KeypadEnum)((i*3)+j);
        }
      }
    }
  }
}

void Keypad::paintEvent(QPaintEvent *) {
  QPainter painter;
  painter.begin(this);

  painter.setFont(QFont("Helvetica", 20, QFont::Bold));

  for(int i=0; i < _width; i++) {
    for(int j=0; j < _width; j++) {
      QRect rect( i*_cellSize, j*_cellSize, _cellSize, _cellSize );

      painter.setPen(QColor("#cccccc"));
      painter.drawRect(rect);
    }
  }

  //currently selected area
  painter.setBrush(QColor("#cccccc"));
  QRect overRect( (_currentIndex/3)*_cellSize,
                  (_currentIndex%3)*_cellSize, _cellSize, _cellSize);
}

void Keypad::playNextTrack() {
}

void Keypad::addTrack(int x, int y, MusicTrack* track) {
}

void Keypad::setGridX(int x) {
  _gridX = x;
}

void Keypad::setGridY(int y) {
  _gridY = y;
}

KeypadSquare *Keypad::getCurrentSquare() {
  return _current;
}

void Keypad::midiXYEvent(unsigned char xaxis, unsigned char yaxis) {
}

void Keypad::keypadClicked(KeypadEnum clicked) {
}

void Keypad::midiPlaylistEvent(bool next) {
}

void Keypad::dragEnterEvent(QDragEnterEvent *event) {
}

void Keypad::dragMoveEvent(QDragMoveEvent *event) {
}

void Keypad::dropEvent(QDropEvent *event) {
}

void Keypad::startDrag(Qt::DropActions supportedActions) {
}

void Keypad::mousePressEvent(QMouseEvent *event) {
  updateXYPosition(event->pos().x(), event->pos().y());

  //if center square is clicked then move to square 1
  //or if it the root then do nothing
  //Go up the tree
  if ( _currentIndex == 4 ) {
    KeypadSquare *parent = _current->getParent();
    if ( parent ) {
      _currentIndex = parent->getChildId(_current);
      _current = parent;
    }
    //Go down the tree
  } else {
    KeypadSquare *child = _current->getChild(_currentIndex);
    if ( !child ) {
      child = _current->createChild(_currentIndex);
    }
    _current = child;
    _currentIndex = KEYPAD4;
  }
  repaint();
}

void Keypad::mouseMoveEvent(QMouseEvent *event) {
  updateXYPosition(event->pos().x(), event->pos().y());
  repaint();
}

/*
 * ============================================================================
 * Keypad Definitions
 *
 * ============================================================================
 */
KeypadSquare::KeypadSquare(KeypadSquare *parent)
  : _parent(parent), _currentTrack(0)
{
  qDebug("Creating Keypad Square\n");
  for(int i=0; i < KEYPAD_SIZE; i++ ) {
    _children[i] = NULL;
  }
}

KeypadSquare::~KeypadSquare() {
  clear();
}

bool KeypadSquare::isEmpty() const {
  return _list.isEmpty();
}

void KeypadSquare::addTrack(MusicTrack *track) {
  if ( track) {
    _list.push_back(track);
  }
}

void KeypadSquare::removeTrack(MusicTrack *track) {
}

void KeypadSquare::nextTrack() {
  _currentTrack++;
  if ( _currentTrack >= _list.size() ) {
    _currentTrack = 0;
  }
}

MusicTrack* KeypadSquare::getCurrent() {
  if ( getCount() > 0 ) {
    MusicTrack *track = _list[_currentTrack];
    return track;
  } else {
    return NULL;
  }
}

MusicTrackIterator KeypadSquare::getTracks() {
  return MusicTrackIterator(_list);
}

int KeypadSquare::getCount() const {
  int total = _list.size();

  for(int i=0; i < KEYPAD_SIZE; i++) {
    if ( _children[i] ) {
      total += _children[i]->getCount();
    }
  }
  return total;
}

void KeypadSquare::clear() {
  _list.clear();
  for(int i=0; i < KEYPAD_SIZE; i++) {
      delete _children[i];
  }
}

KeypadSquare *KeypadSquare::getParent() {
  if ( _parent ) {
    return _parent;
  } else {
    return this;
  }
}

KeypadSquare *KeypadSquare::getChild(KeypadEnum child) {
  if ( child != KEYPAD5 ) {
    return _children[child];
  } else {
    return NULL;
  }
}

KeypadEnum KeypadSquare::getChildId(KeypadSquare *child) {
  for(int i=0; i < KEYPAD_SIZE; i++) {
    if ( child && child == _children[i] ) {
      return (KeypadEnum)(i);
    }
  }
  return KEYPAD_INVALID;
}

KeypadSquare *KeypadSquare::createChild(KeypadEnum child) {
  if ( !_children[child] && child != KEYPAD5 ) {
    _children[child] = new KeypadSquare(this);
  }
  return _children[child];
}



