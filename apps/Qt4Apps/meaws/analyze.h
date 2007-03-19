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

private:
	void getPitches(string audioFilename);
	void getExercise(string exerciseFilename);
	void smoothPitches();
	void writePitches();
	mrs_real findMedian(int start, int length, realvec array);

	int *exercise;
//	float *pitchList;
	realvec pitchList;
	realvec detected;

	int numPitches;
	int exerLength;
};

