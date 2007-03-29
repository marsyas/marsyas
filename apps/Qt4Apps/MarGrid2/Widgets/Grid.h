#ifndef GRID_H
#define GRID_H

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

#include "../Widgets/Tracklist.h"

#define GRID_TRAINING_FILE	"training.mf"
#define GRID_PREDICTION_FILE	"predict.mf"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QPainter;

class Classifier;
class Extractor;
class MarsyasECP;
class GridSquare;

class Grid : public MyDisplay 
{
	Q_OBJECT

public:
	Grid(int winSize, Tracklist *tracklist, QWidget *parent = 0);
	~Grid();
	void clear();

	/*
	void setExtractor(Extractor *extractor);
	Extractor* getExtractor() const; 

	void setClassifier(Classifier *classifier);
	Classifier* getClassifier() const; 
	*/

	int getHeight() const { return _width; }
	int getWidth() const { return _width; }

public slots: 
	void extract();
	void predict();
	void train();
	void midiXYEvent(unsigned char xaxis, unsigned char yaxis);
	void midiPlaylistEvent(bool next);
	void reload();

signals: 
	void playingTrack(MusicTrack *track);
 
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
	GridSquare *getCurrentSquare();

	void setup();

private:
	int _winSize;
	int _cellSize;
	int _gridX;
	int _gridY;
	int _width;

	QVector<GridSquare*> _squares;

	//MarsyasECP *_marsyas;
	//Classifier *_classifier;
	//Extractor *_extractor;

	MusicCollection *_collection; 
};
/*
  MarControlPtr filePtr_;
  
  Marsyas::MarSystemManager mng;  
  QVector<QList <std::string> > files;
  QVector<int> counters;
  QVector<int> counterSizes;
  QVector<int> labels;
  
  
  QList<QPixmap> piecePixmaps;
  QList<QRect> pieceRects;
  QList<QPoint> pieceLocations;

  QRect highlightedRect;
  QRect metalRec;
  QRect classicalRec;
  int inPlace;

  MarSystemWrapper*  _mwr;
  Marsyas::MarSystem* _pnet;

  int _somHeight;
  int _somWidth;
  

  Marsyas::realvec _normSomFmatrix;
  Marsyas::MarSystem* _som;
  Marsyas::MarSystem* _total;
  Marsyas::MarSystem* _norm;
*/ 

class GridSquare
{
public:
	GridSquare(int x, int y);

	bool isEmpty() const;

	void addTrack(MusicTrack* track);

	void nextTrack();
	MusicTrack* getCurrent();
	MusicTrackIterator getTracks();

	int getCount() const;
	int getX() const;
	int getY() const;

	void clear();
	void refresh();

private:
	MusicTrackVector _list;
	int _current;
	int _x;
	int _y;
};

#endif
