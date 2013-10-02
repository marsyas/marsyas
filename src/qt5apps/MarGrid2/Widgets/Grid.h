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

#ifndef GRID_H
#define GRID_H

#include "OSCReceiver.h"
#include "Tracklist.h"
#include "../Interface/MyDisplay.h"
#include "../Interface/Classifier.h"
#include "../Interface/Extractor.h"
#include "../Music/MusicCollection.h"
#include "../Colormaps/Colormap.h"

#include "../../common/marsystem_wrapper.h"
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Collection.h>

#include <ostream>

#include <QList>
#include <QPoint>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QFile>
#include <QDir>
#include <QString>
#include <QTextStream>
#include <QStringList>

#include <ctime>
#include <cstdlib>
#include <map>

#define GRID_TRAINING_FILE	"training.mf"
#define GRID_PREDICTION_FILE	"predict.mf"
#define NUM_GENRES 10

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QPainter;

class Classifier;
class Extractor;
class MarsyasECP;
class GridTriplet;


using namespace MarsyasQt;

class Grid : public QThread
{
  Q_OBJECT

public:
  Grid();
  ~Grid();
  void clear();
  void run();
  void addTrack(int x, int y, QString track);
  void setGridCounter(int index, int value);
  void playTrack(int counter);
  void stopPlaying();
  void removeInitFile();

  QVector< QList<std::string> > getFiles() const { return files_; }
  QList<std::string> getFilesAt(int index);
  QList<std::string> getCurrentFiles();
  QWaitCondition buttonPressed;

  void setXPos(int value);
  void setYPos(int value);
  void setX1Pos(int value);
  void setY1Pos(int value);

  void setXGridSize(QString);
  void setYGridSize(QString);

  void setPlaylist(std::string playlist);
  void addInitFile(QString fileName, int x, int y);
  void setContinuous(bool value);
  bool isContinuous()  {return continuous_; }
  int getGridCounterSizes(int index);
  int getGridCounter(int index);
  int getCurrentIndex();
  QList<std::string> getInitFiles();
  int * getDensity(int index);
  int getHeight()  { return som_height; }
  int getWidth()  { return som_width; }

  int getXPos()  { return _gridX; }
  int getYPos()  { return _gridY; }
  int getX1Pos()  { return _gridX1; }
  int getY1Pos()  { return _gridY1; }

  int getCellSize(int winSize);

  void set_init_alpha(double input) { init_alpha_ = input; }
  void set_init_neighbourhood(double input) {init_neighbourhood_ = input;}
  void set_init_std_factor(double input) {init_std_factor_ = input;}
  void set_init_iterations(int input) {init_iterations_ = input;}
  void set_train_alpha(double input) {train_alpha_ = input;}
  void set_train_neighbourhood(double input) {train_neighbourhood_ = input;}
  void set_train_std_factor(double input) {train_std_factor_ = input;}
  void set_train_iterations(int input) {train_iterations_ = input;}

  double get_init_alpha()  {return init_alpha_;}
  double get_init_neighbourhood()  { return init_neighbourhood_;}
  double get_init_std_factor()  { return init_std_factor_;}
  int get_init_iterations()  { return init_iterations_;}
  double get_train_alpha()  { return train_alpha_;}
  double get_train_neighbourhood()  { return train_neighbourhood_;}
  double get_train_std_factor()  { return train_std_factor_;}
  int get_train_iterations()  { return train_iterations_;}

  OSCReceiver *x0_;
  OSCReceiver *y0_;
  OSCReceiver *x1_;
  OSCReceiver *y1_;

public slots:
  void setExtractMode();
  void setTrainMode();
  void setPredictMode();
  void setInitMode();
  void clearMode();
  void openPredictionGrid(QString fname);
  void savePredictionGrid(QString fname);
  void cancelPressed();
  void openHash();
  void resetGridSlot();

  // sness
  void setTrainFile(QString fname);
  void setValue(int);

signals:
  void playingTrack(MusicTrack *track);
  void repaintSignal();
  void errorBox(QString msg);

protected:
  void resetGrid();

  void setGridX(int x);
  void setGridY(int y);
  void setGridX1(int x);
  void setGridY1(int y);

  void setup();
  void setupNetworks();
  void extract();
  void predict();
  void train();
  void train(bool skipTraining);
  void init();
  void extractAction(std::string filename);


private:
  int _winSize;
  int _cellSize;
  int _gridX;
  int _gridY;
  int _gridX1;
  int _gridY1;
  int som_height;
  int som_width;
  int oldWidth_;
  int oldHeight_;
  int grid_x;
  int grid_y;
  int state_; // 0 for none, 1 for extract, 2 for train, 3 for predict
  int numFeatures;
  int **genreDensity;

  double init_alpha_;
  double init_neighbourhood_;
  double init_std_factor_;
  int init_iterations_;

  double train_alpha_;
  double train_neighbourhood_;
  double train_std_factor_;
  int train_iterations_;

  bool continuous_;
  bool cancel_;
  bool init_;
  std::string playlist_;

  // sness
  std::string trainFile_;

  // Indexed with index = (y * som_hight + x)
  QVector<QList <std::string> > files_;
  QVector<QList <GridTriplet*> > initFileLocations;
  QMutex mutex;

  multimap<string, realvec>* featureHash;
  multimap<string, realvec>* normFeatureHash;
  MusicCollection *_collection;

  MarControlPtr filePtr_;

  Marsyas::MarSystemManager mng;
  QVector<int> counters;
  QVector<int> counterSizes;
  QVector<int> labels;

  Marsyas::MarSystem * m_playMarSystem;
  MarsyasQt::System * m_playSystem;
  MarsyasQt::Control * m_fileNameControl;
  MarsyasQt::Control * m_initAudioControl;

  Marsyas::realvec _normSomFmatrix;
  Marsyas::MarSystem* som_;
  Marsyas::MarSystem* total_;
  Marsyas::MarSystem* norm_;

};

/*
Used to store info about files being used to initialize including the x,y location on the grid
and the file's absolute file path
*/
class GridTriplet
{
public:
  GridTriplet();
  GridTriplet(int, int);
  GridTriplet(std::string, int,int);
  int getX() const {return x;}
  int getY() const {return y;}
  std::string getFileName() const {return fileName;}

  void setX(int);
  void setY(int);
  void setFileName(std::string);

private:
  int x;
  int y;
  std::string fileName;
};

#endif
