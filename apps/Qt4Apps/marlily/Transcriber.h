#ifndef MARSYAS_TRANSCRIBER_H
#define MARSYAS_TRANSCRIBER_H

#include "MarSystemManager.h"
#include "Easymar.h"
using namespace Marsyas;

using namespace std;

class Transcriber
{

public:
	Transcriber();
	~Transcriber();

	static realvec* getPitchesFromAudio(const string audioFilename);
	static MarSystem* makePitchNet(const mrs_real srate, const mrs_real
	                               lowFreq = 100.0, MarSystem* rvSink = NULL);
	static realvec* getPitchesFromRealvecSink(MarSystem* rvSink, const mrs_real
	        srate);
	static MarSystem* makeAmplitudeNet(MarSystem* rvSink = NULL);
	static realvec* getAmpsFromRealvecSink(MarSystem* rvSink);

	static void getAllFromAudio(const string audioFilename, realvec* &
	                            pitchList, realvec* &ampList, realvec*
	                            &boundaries);
	static void toMidi(realvec* pitchList);
	static void pitchSegment(realvec* pitchList, realvec* boundaries);
	static void ampSegment(realvec* ampList, realvec* boundaries);

	static realvec* findPitchBoundaries(const realvec* pitchList);
	static realvec* findValleys(const realvec* list);
	static void findAmpBoundaries(realvec* ampList, realvec*
&boundaries);
	static mrs_real findNextPeakValue(const realvec* list, const mrs_natural
start);

	static mrs_real findMedian(const mrs_natural start, const
	                           mrs_natural length, const realvec* array);

	static realvec* segmentRealvec(const realvec* list, const realvec* boundaries);
	static void appendRealvec(realvec* orig, const realvec* newValues);
	static realvec* getSubVector(const realvec* list, const mrs_natural
	                             start, const mrs_natural length);
	static realvec* getSubVectorNoZeros(const realvec* list, const mrs_natural
	                             start, const mrs_natural length);


	static realvec* getNotes(const realvec* pitchList, const realvec*
	                         ampList, const realvec* boundaries);
	static void getRelativeDurations(const realvec *boundaries,
realvec* &durations);

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

