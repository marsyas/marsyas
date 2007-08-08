#ifndef MARSYAS_TRANSCRIBER_H
#define MARSYAS_TRANSCRIBER_H

#include "MarSystemManager.h"
#include "Easymar.h"
using namespace Marsyas;

using namespace std;

class Transcriber {

public:
    Transcriber();
    ~Transcriber();

    static realvec getPitchesFromAudio(const string audioFilename);
    static MarSystem* makePitchNet(const mrs_real srate, const mrs_real lowFreq);
	static realvec getPitchesFromRealvecSink(MarSystem* rvSink, const mrs_real
srate);
	static MarSystem* makeAmplitudeNet();

	static void getAllFromAudio(const string audioFilename);

    /*
    	void setOptions(mrs_natural getRadius, mrs_real getNewNote, mrs_real getCertantyDiv);

    	void setPitchList(realvec newPitchList);
    	realvec getAmplitudesFromAudio(string audioFilename);

    	void calcOnsets();
    	void calcRelativeDurations();
    	void calcNotes();

    	realvec getOnsets();
    	realvec getDurations();
    	realvec getNotes();

    	void clearPitches();
    	void setOnsets(string fileName);
    */

private:
    /*
    	mrs_real findMedian(mrs_natural start, mrs_natural length, realvec array);
    	mrs_real notePitch(realvec curNote);

    	string outputFilename;
    	realvec pitchList;
    	realvec ampList;
    	realvec fretList;

    	realvec onsets;
    	realvec durations;
    	realvec notes;

    	mrs_natural median_radius;
    	mrs_real new_note_midi;
    	mrs_real pitch_certainty_div;

    	mrs_natural IMPULSE_HIGHT;
    	mrs_natural LOWEST_NOTE;
    	mrs_natural HIGHEST_NOTE;
    */
};
#endif

