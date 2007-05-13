// partly-old file, partly-new.  Will probably be merged
// with backend .h.cpp  and/or main marsyas code

#ifndef MEAWS_ANALYZE_H
#define MEAWS_ANALYZE_H

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

	void prepSaxPitches();
	realvec *retPitches();
	double getPitchStability();
	realvec *retAmplitudes();

	void initHarms();
	void addHarmsHokey();
	void addHarmsSmooth();
	void addHarmsBasic();

	void makeMinor();
	void screwJazz();

private:
	void getPitches(string audioFilename);
	void getAmplitudes(string audioFilename);
	void getExercise(string exerciseFilename);
	void smoothPitches();
	void writeTemp(realvec temp);
	mrs_real findMedian(int start, int length, realvec array);

	int *exercise;
	realvec pitchList;
	realvec ampList;
	realvec detected;
	bool PITCH_CORRECT;
	string outputFileName;

	mrs_natural numPitches;
	mrs_natural exerLength;
};
#endif

