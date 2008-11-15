
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
#include <QFile>
#include <QDir>
#include <QTextStream>

#include <ctime>
#include <cstdlib>
#include <map>

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
	void addTrack(int x, int y, std::string track);
	void setGridCounter(int index, int value);
	void playTrack(int counter);
	void stopPlaying();
	QList<std::string> getFilesAt(int index);
	QList<std::string> getCurrentFiles();
	QWaitCondition buttonPressed;
	
	void setXPos(int value);
	void setYPos(int value);
	void addInitFile(QString fileName, int x, int y);
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
		void setInitMode();
		void clearMode();
		void openPredictionGrid(QString fname);
		void savePredictionGrid(QString fname);
		void cancelPressed();
		void openNormHash();

signals: 
		void playingTrack(MusicTrack *track);
		void repaintSignal();

protected:
	void resetGrid();

	void setGridX(int x);  
	void setGridY(int y); 

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
	int som_height;
	int som_width;
	int grid_x;
	int grid_y;
	int state_; // 0 for none, 1 for extract, 2 for train, 3 for predict
	int oldPlayingIndex;
	int numFeatures;
	bool initAudio_;
	bool continuous_;
	bool cancel_;
	bool init_;



	QVector<QList <std::string> > files_;
	QVector<GridTriplet*> initFileLocations;
	QMutex mutex;

	multimap<string, realvec>* featureHash;
	multimap<string, realvec>* normFeatureHash;
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

/*
Used to store info about files being used to initialize including the x,y location on the grid
as well as column of the feature matrix
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
