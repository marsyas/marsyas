#include "MarSystemManager.h"
using namespace Marsyas;

using namespace std;

class Analyze {

public:
	Analyze(string getaudioFilename, string exerciseFilename);
	~Analyze();
	void calcDurations();
	void calcNotes();
	void writeNotes();
	void printNotes();
	void writePitches();

	void addHarmonies();

private:
	void getPitches(string audioFilename);
	void getExercise(string exerciseFilename);
	void smoothPitches();
	mrs_real findMedian(int start, int length, realvec array);

	int *exercise;
	realvec pitchList;
	realvec detected;

	mrs_natural numPitches;
	mrs_natural exerLength;
};

