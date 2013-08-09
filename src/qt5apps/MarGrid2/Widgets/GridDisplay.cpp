#include "GridDisplay.h"

#include <QMimeData>

GridDisplay::GridDisplay(int winSize, Tracklist *tracklist, Grid* grid_, QWidget *parent)
  : MyDisplay(tracklist, parent), _winSize(winSize)
{
  this->grid_ = grid_;
  _cellSize = grid_->getCellSize(_winSize);
  setMinimumSize(winSize, winSize);
  setMouseTracking(true);
  setAcceptDrops(true);
  squareHasInitialized.resize(grid_->getHeight() * grid_->getWidth());
  for(int i = 0; i < grid_->getHeight() * grid_->getWidth(); i++)
    squareHasInitialized[i] = false;
  oldXPos = -1;
  oldYPos = -1;
  oldX1Pos = -1;
  oldY1Pos = -1;
  fullScreenMouseOn = false;
  initDone = false;
  fullScreenTimer = new QTimer(this);
  fullScreenTimer->setInterval(150);
  colourMapMode_ = false;


  connect(this, SIGNAL(clearMode()), grid_, SLOT(clearMode()));
  connect(this, SIGNAL(extractMode()), grid_, SLOT(setExtractMode()));
  connect(this, SIGNAL(trainMode()), grid_, SLOT(setTrainMode()));
  connect(this, SIGNAL(predictMode()), grid_, SLOT(setPredictMode()));
  connect(this, SIGNAL(initMode()), grid_, SLOT(setInitMode()));
  connect(this, SIGNAL(savePredictionGridSignal(QString)), grid_, SLOT(savePredictionGrid(QString)));
  connect(this, SIGNAL(openPredictionGridSignal(QString)), grid_, SLOT(openPredictionGrid(QString)));
  connect(grid_, SIGNAL(repaintSignal()), this, SLOT(repaintSlot()));
  connect(this, SIGNAL(cancelButtonPressed()), grid_, SLOT(cancelPressed()));
  connect(this, SIGNAL(hashLoadPressed()), grid_, SLOT(openHash()));
  connect(fullScreenTimer, SIGNAL(timeout()), this, SLOT(fullScreenMouseMove()));
  connect(grid_, SIGNAL(errorBox(QString)), this, SLOT(showErrorMessage(QString)));
  connect(this, SIGNAL(resetGridAction()), grid_, SLOT(resetGridSlot()));
}

GridDisplay::~GridDisplay()
{
  delete fullScreenTimer;

}
/*
*
* SLOTS
*
*/

void GridDisplay::clear()
{

  emit clearMode();

}
void GridDisplay::train()
{
  emit trainMode();
  grid_->buttonPressed.wakeAll();
}
void GridDisplay::extract()
{
  cout << "extract" <<endl;
  emit extractMode();
  grid_->buttonPressed.wakeAll();
}
void GridDisplay::predict()
{
  emit predictMode();
  grid_->buttonPressed.wakeAll();
}
void GridDisplay::init()
{
  emit initMode();
  grid_->buttonPressed.wakeAll();
  initDone = true;
}
void GridDisplay::savePredictionGrid(QString fname)
{
  emit savePredictionGridSignal(fname);
}
void GridDisplay::openPredictionGrid(QString fname)
{
  emit openPredictionGridSignal(fname);
}
void GridDisplay::playModeChanged()
{
  grid_->setContinuous( !grid_->isContinuous() );
}
void GridDisplay::repaintSlot()
{
  repaint();
}
void GridDisplay::cancelButton()
{
  emit cancelButtonPressed();
}

void GridDisplay::hashLoad()
{
  emit hashLoadPressed();
}

void GridDisplay::resetGrid()
{
  initDone = false;
  for(int i = 0; i < squareHasInitialized.size(); i++)
    squareHasInitialized[i] = false;
  emit resetGridAction();
}

void GridDisplay::fullScreenMouse()
{
  if(!fullScreenMouseOn)
  {
    fullScreenTimer->start();
    grabMouse();
    fullScreenMouseOn = true;
  }
  else
  {
    fullScreenTimer->stop();
    releaseMouse();
    fullScreenMouseOn = false;
  }
  emit fullScreenMode(fullScreenMouseOn);
  setMouseTracking(!fullScreenMouseOn);

}
void GridDisplay::fullScreenMouseMove()
{
  QRect screenSize = (QApplication::desktop())->screenGeometry();

  // map mouse position on screen to a grid location
  int gridX = qRound(mouseCursor->pos().x() / (screenSize.width() / ((float)grid_->getWidth() - 0.5)));
  int gridY = qRound(mouseCursor->pos().y() / (screenSize.height() / ((float)grid_->getHeight() - 0.5)));

  if(lastMousePoint != mouseCursor->pos())updateXYPosition(gridX, gridY);
  lastMousePoint = mouseCursor->pos();

  if(grid_->getXPos() != oldXPos || oldYPos != grid_->getYPos()  )
  {

    playNextTrack();
    oldXPos = grid_->getXPos();
    oldYPos = grid_->getYPos();
    repaint();
  }
}

void GridDisplay::keyMove(QKeyEvent *keyEvent)
{

  int gridX = grid_->getXPos();
  int gridY = grid_->getYPos();
  bool reachedEdge;

// cout<<"GridDisplay::keyMove grid x, y: "<<gridX<<", "<<gridY<<endl;

  // Up key pressed
  if (keyEvent->key() == Qt::Key_Up) {
    gridY -= 1;
    if (gridY < 0) {
      reachedEdge = true;
      QApplication::beep();
      //gridY = grid_->getHeight() - 1;

      gridY = 0;
    } else reachedEdge = false;
  }

  // Down key pressed
  if (keyEvent->key() == Qt::Key_Down) {
    gridY += 1;
    if (gridY > grid_->getHeight() - 1) {
      reachedEdge = true;
      QApplication::beep();
      // gridY = 0;
      gridY = getHeight() - 1;
    } else reachedEdge = false;
  }

  // Left key pressed
  if (keyEvent->key() == Qt::Key_Left) {
    gridX -= 1;
    if (gridX < 0) {
      reachedEdge = true;
      QApplication::beep();
      //gridX = grid_->getHeight() - 1;
      gridX = 0;
    } else reachedEdge = false;
  }

  // Right key pressed
  if (keyEvent->key() == Qt::Key_Right) {
    gridX += 1;
    if (gridX > grid_->getWidth() - 1) {
      reachedEdge = true;
      QApplication::beep();
      //gridX = 0;
      gridX = grid_->getWidth()-1;
    } else reachedEdge = false;
  }

  // "q" key pressed - Go to the top left grid point
  if (keyEvent->key() == 81) {
    reachedEdge = false;
    gridX = 0;
    gridY = 0;
  }

  // "w" key pressed - Go to the top right grid point
  if (keyEvent->key() == 87) {
    reachedEdge = false;
    gridX = grid_->getWidth() - 1;
    gridY = 0;
  }

  // "a" key pressed - Go to the bottom left grid point
  if (keyEvent->key() == 65) {
    reachedEdge = false;
    gridX = 0;
    gridY = grid_->getHeight() - 1;
  }

  // "s" key pressed - Go to the bottom right grid point
  if (keyEvent->key() == 83) {
    reachedEdge = false;
    gridX = grid_->getWidth() - 1;;
    gridY = grid_->getHeight() - 1;
  }

  // "x" key pressed - Go to the center grid point
  if (keyEvent->key() == 88) {
    reachedEdge = false;
    gridX = (grid_->getWidth() - 1) / 2;
    gridY = (grid_->getHeight() - 1) / 2;
  }


  if(!reachedEdge && keyEvent->key() != Qt::Key_Shift)
  {
    updateXYPosition(gridX, gridY);

    playNextTrack();
    oldXPos = grid_->getXPos();
    oldYPos = grid_->getYPos();
    repaint();
  } else if(keyEvent->key() == Qt::Key_Shift) {
    playNextTrack();
  }


}

void GridDisplay::colourMapMode()
{
  colourMapMode_ = !colourMapMode_;
  repaint();
}

void GridDisplay::playlistSelected(QString playlist)
{
  grid_->setPlaylist(playlist.toStdString());
}

void GridDisplay::setXGridSize(QString size)
{
  grid_->setXGridSize(size);
  squareHasInitialized.clear();
  squareHasInitialized.resize(grid_->getHeight() * grid_->getWidth());
  for(int i = 0; i < grid_->getHeight() * grid_->getWidth(); i++)
    squareHasInitialized[i] = false;
}
void GridDisplay::setYGridSize(QString size)
{
  grid_->setYGridSize(size);
  squareHasInitialized.clear();
  squareHasInitialized.resize(grid_->getHeight() * grid_->getWidth());
  for(int i = 0; i < grid_->getHeight() * grid_->getWidth(); i++)
    squareHasInitialized[i] = false;
}

void GridDisplay::showErrorMessage(QString msg)
{
  QMessageBox::information(0, tr("MarGrid"), tr(msg.toStdString().c_str()));
}

// ******************************************************************
//
// FUNCTIONS
void GridDisplay::midiXYEvent(unsigned char xaxis, unsigned char yaxis)
{
  int x = (int)(xaxis / 128.0 * grid_->getWidth());
  int y = grid_->getHeight() - 1 - (int)(yaxis / 128.0 * grid_->getHeight());

  std::cout << "midi xy event (" << x << "," << y << ")\n";
  updateXYPosition(x, y);
  playNextTrack();
}

void GridDisplay::updateXYPosition(int x, int y)
{
  grid_->setXPos(x);
  grid_->setYPos(y);

}

void GridDisplay::updateXY1Position(int x, int y)
{
  grid_->setX1Pos(x);
  grid_->setY1Pos(y);

}

void GridDisplay::midiPlaylistEvent(bool next)
{
  if ( next ) {
    std::cout << "midi playlist event\n";
    playNextTrack();
  }
}

void GridDisplay::reload()
{

  //TODO:  Figure out if reload is needed
}


void GridDisplay::playNextTrack()
{
  if( !grid_->getCurrentFiles().isEmpty() )
  {
    //get number of tracks in current grid square
    int counterSize = grid_->getGridCounterSizes(grid_->getCurrentIndex());
    if (counterSize > 0 && lastIndex == grid_->getCurrentIndex()) {
      //set the next song to play in current grid square reset to first song
      grid_->setGridCounter( grid_->getCurrentIndex() , (grid_->getGridCounter(grid_->getCurrentIndex()) + 1) % counterSize );
    } else if(lastIndex != grid_->getCurrentIndex()) { //we changed squares
      grid_->setGridCounter( grid_->getCurrentIndex() , 0);
    }
    lastIndex = grid_->getCurrentIndex();

    int counter = grid_->getGridCounter(grid_->getCurrentIndex());
    QList<std::string> playlist = grid_->getCurrentFiles();
    cout << "Currently Playing: " + playlist[counter] << endl;
    cout << "Playlist: " << endl;
    for(int i = 0; i < counterSize; i++ ) {
      cout << playlist[i] << endl;
    }

    grid_->playTrack(counter);
  } else
  {
    grid_->stopPlaying();
  }
}


/*
* -----------------------------------------------------------------------------
* Event Handlers
* -----------------------------------------------------------------------------
*/

bool GridDisplay::event(QEvent *event)
{
  if (event->type() == QEvent::ToolTip)
  {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    int k = grid_->getYPos() * grid_->getHeight() + grid_->getXPos();
    if(squareHasInitialized[k])
    {
      QList<std::string> initFiles = grid_->getInitFiles();
      QString initFileStr = "";
      for(int i = 0; i < initFiles.size(); i++)
      {
        std::string temp = initFiles.at(i);
        initFileStr += temp.c_str();
        initFileStr += "\n";
      }
      QToolTip::showText(helpEvent->globalPos(), initFileStr);
    }
    else
    {
      QToolTip::hideText();
    }
  }
  return QWidget::event(event);
}

void GridDisplay::mousePressEvent(QMouseEvent *event)
{
  std::cout << "mouse Press Event" << std::endl;

  if(fullScreenMouseOn)
  {
    QRect screenSize = (QApplication::desktop())->screenGeometry();
    int gridX = mouseCursor->pos().x() / (screenSize.width() / (grid_->getWidth() - 1));
    int gridY = mouseCursor->pos().y() / (screenSize.height() / (grid_->getHeight() - 1));

    if(lastMousePoint != mouseCursor->pos())updateXYPosition(gridX, gridY);
    lastMousePoint = mouseCursor->pos();

    if(grid_->getXPos() != oldXPos || oldYPos != grid_->getYPos()  )
    {
      oldXPos = grid_->getXPos();
      oldYPos = grid_->getYPos();
    }
  }
  else
  {
    if(lastMousePoint != mouseCursor->pos()) {
      updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);
    }
    lastMousePoint = mouseCursor->pos();
    if(!initDone)
    {
      int k = grid_->getYPos() * grid_->getHeight() + grid_->getXPos();
      if(squareHasInitialized[k])
      {
        squareHasInitialized[k] = false;
        grid_->removeInitFile();
      }
    }
  }
  playNextTrack();
  repaint();
}

void GridDisplay::mouseMoveEvent(QMouseEvent *event)
{

  if ( (event->pos().x() >= _winSize) || (event->pos().y() >= _winSize) ) {
    return;
  }
  QToolTip::hideText();
  updateXYPosition(event->pos().x() / _cellSize, event->pos().y() / _cellSize);
  lastMousePoint = mouseCursor->pos();


  if(grid_->isContinuous() && (grid_->getXPos() != oldXPos || oldYPos != grid_->getYPos() ) )
  {
    playNextTrack();
    oldXPos = grid_->getXPos();
    oldYPos = grid_->getYPos();
  }
  emit updateColourMap(grid_->getDensity(grid_->getCurrentIndex()), 10);
  repaint();
}

void GridDisplay::dragMoveEvent(QDragMoveEvent* event )
{


  //TODO: NEED TO ADD IN FOR INIT

  //qDebug() << "Drag Move";
}

void GridDisplay::dragEnterEvent(QDragEnterEvent* event)
{
  if ( event->proposedAction() == Qt::CopyAction ) {
    event->acceptProposedAction();
  }
}

void GridDisplay::dropEvent(QDropEvent *event)
{

  qDebug() << "drop!!" ;
  if ( event->proposedAction() == Qt::CopyAction )
  {
    //Position of drop event
    int x = event->pos().x() / _cellSize;
    int y = event->pos().y() / _cellSize;
    int k = y * grid_->getHeight() + x;
    squareHasInitialized[k] = true;


    const QMimeData* data = event->mimeData();

    QString trackName = data->text();

    QByteArray trackLocation = data->data("application/track-location");
    QString foobar = trackLocation;

    if ( data->hasFormat("application/track-id") ) {
      cout << "Track" << endl;
      grid_->addInitFile(trackLocation, x, y);
    } else if ( data->hasFormat("application/playlist-id") ) {
      cout << "Play List" << endl;
    }
  }
  repaint();
}

void GridDisplay::paintEvent(QPaintEvent* /* event */)
{

  QPainter painter;
  painter.begin(this);
  _cellSize = grid_->getCellSize(_winSize);

  //Find density
  int maxDensity = 0;
  int minDensity = 100;
  for (int i=0; i < grid_->getFiles().size(); i++) {
    if(grid_->getFilesAt(i).size() > maxDensity)
    {
      maxDensity = grid_->getFilesAt(i).size();
    }
    else if (grid_->getFilesAt(i).size() < minDensity)
    {
      minDensity = grid_->getFilesAt(i).size();
    }
  }
  Colormap *map = Colormap::factory(Colormap::GreyScale);
  for (int i=0; i < grid_->getHeight(); i++) {
    for (int j=0; j < grid_->getWidth(); j++) {

      int k = j * grid_->getHeight() + i;

      QRect	 myr(i*_cellSize,j*_cellSize,_cellSize,_cellSize);
      QLine	 myl1(i*_cellSize,j*_cellSize, i*_cellSize, j*_cellSize + _cellSize);
      QLine	 myl2(i*_cellSize,j*_cellSize, i*_cellSize+_cellSize, j*_cellSize );

      if ( grid_->getFilesAt(k).size() == 0 )
      {
        QColor color;
        if(colourMapMode_)
        {
          color.setRgb(map->getRed(0), map->getGreen(0), map->getBlue(0));
        }
        else
        {
          color.setRgb(map->getRed(128), map->getGreen(0), map->getBlue(0));
        }
        painter.setBrush(color);
      }
      else
      {
        if(colourMapMode_)
        {
          /*
          * index - genre - color
          * 0 - blues - Qt::blue
          * 1 - classical - Qt::darkRed
          * 2 - country - Qt::green
          * 3 - disco - PURPLE
          * 4 - hiphop - Qt::yellow
          * 5 - jazz - Qt::darkGreen
          * 6 - metal - BROWN
          * 7 - reggae - PINK
          * 8 - rock - ORANGE
          * 9 - pop - Qt::grey
          */
          int * genreDensity = grid_->getDensity(k);
          double *squarePaintSize = new double[10];
          int startHeight = j*_cellSize;

          for(int m = 0; m < 10; m++)
          {
            squarePaintSize[m] = genreDensity[m] /  (1.0 * grid_->getFilesAt(k).size() );
          }

          // 10 is the number of genres
          for(int l = 0; l < 10; l++)
          {
            QColor * color;
            switch(l)
            {
            case 0:
              color = new QColor(Qt::blue);
              break;
            case 1:
              color = new QColor(Qt::darkRed);
              break;
            case 2:
              color = new QColor(Qt::green);
              break;
            case 3:
              color = new QColor(PURPLE);
              break;
            case 4:
              color = new QColor(Qt::yellow);
              break;
            case 5:
              color = new QColor(Qt::darkGreen);
              break;
            case 6:
              color = new QColor(BROWN);
              break;
            case 7:
              color = new QColor(PINK);
              break;
            case 8:
              color = new QColor(ORANGE);
              break;
            case 9:
              color = new QColor(Qt::gray);
              break;
            }
            if(grid_->getFilesAt(k).size() > 0)
            {
              painter.setBrush(*color);
              painter.setPen(Qt::NoPen);
              QRect rect(i*_cellSize, startHeight, _cellSize, squarePaintSize[l] * _cellSize);
              painter.drawRect(rect);
              startHeight += squarePaintSize[l] *  _cellSize;
            }
          }

        }
        else
        {
          int c = int(grid_->getFilesAt(k).size() / float(maxDensity) * (map->getDepth()-1));
          QColor color(map->getRed(c), map->getGreen(c), map->getBlue(c));
          painter.setBrush(color);
        }
      }

      if(colourMapMode_)
      {
        painter.setPen(Qt::white);
        if(grid_->getFilesAt(k).size() == 0)
          painter.drawRect(myr);
      }
      else
      {
        painter.setPen(Qt::NoPen);
        painter.drawRect(myr);
        painter.setPen(Qt::black);
      }
      painter.drawLine(myl1);
      painter.drawLine(myl2);

      painter.setBrush(Qt::red);
      QRect newr( grid_->getXPos() * _cellSize + _cellSize / 4,
                  grid_->getYPos() * _cellSize + _cellSize / 4,
                  _cellSize - _cellSize / 2,
                  _cellSize-_cellSize / 2);
      painter.drawRect(newr);

      painter.setBrush(Qt::white);
      QRect newr1( grid_->getX1Pos() * _cellSize + _cellSize / 4,
                   grid_->getY1Pos() * _cellSize + _cellSize / 4,
                   _cellSize - _cellSize / 2,
                   _cellSize-_cellSize / 2);
      painter.drawRect(newr1);
    }
  }

  // Draw an 'i' in all initilized squares
  for(int i = 0; i < squareHasInitialized.size(); i++)
  {
    if(squareHasInitialized[i])
    {
      int y = i / grid_->getHeight();
      int x = i % grid_->getHeight();
      painter.setBrush(Qt::green);
      QFont *font = new QFont();
      font->setPointSize(16);
      font->setStyleHint(QFont::OldEnglish);
      painter.setFont(*font);
      painter.drawText(x * _cellSize, y * _cellSize, _cellSize, _cellSize, Qt::AlignCenter, "i");
    }
  }

  delete map;
  painter.end();
}
