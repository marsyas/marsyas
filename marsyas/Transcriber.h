#ifndef MARSYAS_TRANSCRIBER_H
#define MARSYAS_TRANSCRIBER_H

#include "MarSystemManager.h"
using namespace Marsyas;

using namespace std;

class Transcriber {

public:
	Transcriber();
	~Transcriber();
	void setOptions(mrs_natural getRadius, mrs_real getNewNote, mrs_real getCertantyDiv);

	void setPitchList(realvec newPitchList);
	void getPitchesFromAudio(string audioFilename);

	void calcOnsets();
	void calcNotes();

	realvec getOnsets();
	realvec getNotes();

	void clearPitches();
	void setOnsets(string fileName);

private:
	mrs_real findMedian(int start, int length, realvec array);
	mrs_real notePitch(realvec curNote);

	string outputFilename;
	realvec pitchList;
	realvec fretList;

	realvec onsets;
	realvec notes;

	mrs_natural median_radius;
	mrs_real new_note_midi;
	mrs_real pitch_certainty_div;

	int IMPULSE_HIGHT;
	int LOWEST_NOTE;
	int HIGHEST_NOTE;
};
#endif

