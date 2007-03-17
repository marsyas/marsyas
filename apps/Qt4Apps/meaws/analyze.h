//#include <QObject>
#include "MarSystemManager.h"
using namespace Marsyas;

using namespace std;

class Analyze {
//class Analyze: public QObject {
//	Q_OBJECT

public:
	Analyze(string getaudioFilename, string exerciseFilename);
	~Analyze();
	void calcDurations();
	void calcNotes();
	void metroDurations();

/*
signals:
	void nextNoteError(float error, int direction);
*/
private:
	void getPitches(string audioFilename);
	void getExercise(string exerciseFilename);
	void smoothPitches();

	int *exercise;
	float *pitchList;
	float *detected;

	int numPitches;
	int exerLength;
};

