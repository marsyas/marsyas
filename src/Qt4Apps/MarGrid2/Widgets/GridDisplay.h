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

#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
#include "Collection.h"

#include "../Music/MusicCollection.h"
#include "../Colormaps/Colormap.h"

#include "../Widgets/Tracklist.h"
#include "../Widgets/Grid.h"

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

	public slots: 
		void midiXYEvent(unsigned char xaxis, unsigned char yaxis);
		void midiPlaylistEvent(bool next);
		void reload();
				void extract();
		void predict();
		void train();
		void openPredictionGrid(QString fname);
		void savePredictionGrid(QString fname);
		void playModeChanged();

signals: 
		void playingTrack(MusicTrack *track);
		void extractMode();
		void trainMode();
		void predictMode();
		void clearMode();

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

private:
	Grid* grid_;

	QList<QPixmap> piecePixmaps;
	QList<QRect> pieceRects;
	QList<QPoint> pieceLocations;
	QWaitCondition buttonPressed;


	QRect highlightedRect;
	QRect metalRec;
	QRect classicalRec;
	int inPlace;
    int _winSize;
	int _cellSize;

};

#endif