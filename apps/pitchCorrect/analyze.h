//  TEMPORARY -- these functions will soon be implemented as
//  MarSystems.

#include "MarSystemManager.h"
using namespace Marsyas;

using namespace std;

class Analyze {

public:
	Analyze();
	~Analyze();

	void setOptions(mrs_real getRadius, mrs_real getNewNote, mrs_real getCertantyDiv);


	void loadData(string inputFilename);

	void calcOnsets();
	void calcNotes();

	void clearPitches();

	void writePitches();
	void writeOnsets();
	void writeNotes();

private:
	void getPitchesFromAudio(string audioFilename);
	void getPitchesFromRealvecText(string textFilename); // for Sitar fret data
	void getExpectedNotes(string expectedNotesFilename);
	mrs_real findMedian(int start, int length, realvec array);

	string outputFilename;
	realvec pitchList;

	realvec onsets;
	realvec notes;

	mrs_natural median_radius;
	mrs_real new_note_midi;
	mrs_real pitch_certainty_div;

	int IMPULSE_HIGHT;
	int LOWEST_NOTE;
	int HIGHEST_NOTE;
};

