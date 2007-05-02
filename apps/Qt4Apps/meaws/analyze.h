#include "MarSystemManager.h"
using namespace Marsyas;

using namespace std;

class Analyze {

public:
	Analyze(string getaudioFilename, string exerciseFilename);
	~Analyze();
	void calcDurations();
	void calcNotes();
	void calcMultipliers();
	void calcIndividualMultipliers();
	void calcNothing();
	void writeNotes();
	void writeHarmData();
	void printNotes();
	void writePitches();

	realvec *retPitches();
	double getPitchStability();

	void initHarms();
	void addHarmsHokey();
	void addHarmsSmooth();
	void addHarmsBasic();

	void makeMinor();
	void screwJazz();

private:
	void getPitches(string audioFilename);
	void getExercise(string exerciseFilename);
	void smoothPitches();
	void writeTemp(realvec temp);
	mrs_real findMedian(int start, int length, realvec array);

	int *exercise;
	realvec pitchList;
	realvec detected;
	bool PITCH_CORRECT;
	string outputFileName;

	mrs_natural numPitches;
	mrs_natural exerLength;
};

