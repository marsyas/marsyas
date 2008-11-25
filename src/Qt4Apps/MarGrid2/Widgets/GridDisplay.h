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
		void init();
		void openPredictionGrid(QString fname);
		void savePredictionGrid(QString fname);
		void playModeChanged();
		void cancelButton();
		void repaintSlot();
		void normHashLoad();
		void fullScreenMouse();
		void fullScreenMouseMove();

signals: 
		void playingTrack(MusicTrack *track);
		void extractMode();
		void trainMode();
		void predictMode();
		void initMode();
		void clearMode();
        void openPredictionGridSignal(QString fname);
		void savePredictionGridSignal(QString fname);
		void cancelButtonPressed();
		void normHashLoadPressed();
		void fullScreenMode(bool modeOn);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
	void startDrag(Qt::DropActions supportedActions);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	bool event(QEvent *event);

	void updateXYPosition(int x, int y); 
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


	QRect highlightedRect;
	QRect metalRec;
	QRect classicalRec;
	int inPlace;
    int _winSize;
	int _cellSize;
	int oldXPos;
	int oldYPos;
	bool fullScreenMouseOn;
	bool initDone;

};

#endif