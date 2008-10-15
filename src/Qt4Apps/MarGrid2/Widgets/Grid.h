
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
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include <ctime>
#include <cstdlib>

#include "../Interface/MyDisplay.h"
#include "../Interface/Classifier.h"
#include "../Interface/Extractor.h"
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
#include "Collection.h"

#include "../Music/MusicCollection.h"
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


using namespace MarsyasQt;

class Grid : public QThread
{
	Q_OBJECT

public:
	Grid();
	~Grid();
	void clear();
	void run();
	void addTrack(int x, int y, std::string track);
	void setGridCounter(int index, int value);
	void playTrack(int counter);
	QList<std::string> getFilesAt(int index);
	QList<std::string> getCurrentFiles();
	QWaitCondition buttonPressed;
	
	void setXPos(int value);
	void setYPos(int value);
	void setContinuous(bool value);
	bool isContinuous() const {return continuous_; }
	int getGridCounterSizes(int index);
	int getGridCounter(int index);
	int getCurrentIndex();
	int getHeight() const { return som_width; }
	int getWidth() const { return som_height; }
	int getXPos() const { return _gridX; }
	int getYPos() const { return _gridY; }
	int getCellSize() const { return _cellSize; }
	QVector< QList<std::string> > getFiles() const { return files_; }

public slots: 
		void setExtractMode();
		void setTrainMode();
		void setPredictMode();
		void clearMode();
		//void midiXYEvent(unsigned char xaxis, unsigned char yaxis);
		//void midiPlaylistEvent(bool next);
		//void reload();
		void openPredictionGrid(QString fname);
		void savePredictionGrid(QString fname);

signals: 
		void playingTrack(MusicTrack *track);

protected:
	void resetGrid();

	void setGridX(int x);  
	void setGridY(int y); 

	void setup();
	void setupNetworks();
	void extract();
	void predict();
	void train();


private:
	int _winSize;
	int _cellSize;
	int _gridX;
	int _gridY;
	int som_height;
	int som_width;
	int grid_x;
	int grid_y;
	int state_; // 0 for none, 1 for extract, 2 for train, 3 for predict
	bool initAudio_;
	bool continuous_;


	QVector<QList <std::string> > files_;
	QMutex mutex;

	MusicCollection *_collection;

	MarControlPtr filePtr_;

	Marsyas::MarSystemManager mng;  
	QVector<int> counters;
	QVector<int> counterSizes;
	QVector<int> labels;

	MarSystemQtWrapper*  mwr_;
    Marsyas::MarSystem* pnet_;

	Marsyas::realvec _normSomFmatrix;
	Marsyas::MarSystem* som_;
	Marsyas::MarSystem* total_;
	Marsyas::MarSystem* norm_;

};

#endif
