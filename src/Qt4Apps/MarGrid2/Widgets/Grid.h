
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
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
#include "Collection.h"

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


using namespace MarsyasQt;

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

	int getHeight() const { return gridWidth_; }
	int getWidth() const { return gridHeight_; }

	public slots: 
		void extract();
		void predict();
		void train();
		void midiXYEvent(unsigned char xaxis, unsigned char yaxis);
		void midiPlaylistEvent(bool next);
		void reload();
		void openPredictionGrid(QString fname);
		void savePredictionGrid(QString fname);

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

	void addTrack(int x, int y, std::string track);
	void resetGrid();

	void setGridX(int x);  
	void setGridY(int y); 
	GridSquare *getCurrentSquare();

	void setup();
	void setupNetworks();
	int getCurrentIndex();

private:
	int _winSize;
	int _cellSize;
	int _gridX;
	int _gridY;
	int gridHeight_;
	int gridWidth_;
	int som_height;
	int som_width;
	int grid_x;
	int grid_y;
	bool initAudio_;
	bool continuous_;

	QVector<GridSquare*> _squares;
	QVector<QList <std::string> > files_;

	MusicCollection *_collection;

	MarControlPtr filePtr_;

	Marsyas::MarSystemManager mng;  
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

	MarSystemQtWrapper*  mwr_;
    Marsyas::MarSystem* pnet_;

	Marsyas::realvec _normSomFmatrix;
	Marsyas::MarSystem* som_;
	Marsyas::MarSystem* total_;
	Marsyas::MarSystem* norm_;

};
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
