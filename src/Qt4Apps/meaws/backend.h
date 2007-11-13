#ifndef MEAWS_BACKEND_H
#define MEAWS_BACKEND_H

#include "defs.h"
#include <QObject>
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
#include "TranscriberExtract.h"
using namespace Marsyas;
using namespace MarsyasQt;

class MarBackend: public QObject {
	Q_OBJECT

public:
	MarBackend(int getType);
	~MarBackend();

//	void setFileName(std::string filename);
	void playFile();
	void openTry(std::string filename);
	void newTry();

	bool analyze();
	realvec getPitches();
	realvec getMidiPitches();
	realvec getAmplitudes();

// transcriber
//	realvec getDurations();
//	realvec getNotes();

	mrs_real getRate();

public slots:
	void ctrlChanged(MarControlPtr changed);
	void start();
	void stop();

signals:
	void setAttempt(bool running);
	void gotAudio();

private:
	MarSystemManager mng;
	MarSystem *sourceNet;
	MarSystem *allNet;
	MarSystem *pitchSink;
	MarSystem *ampSink;
	MarSystemQtWrapper *mrsWrapper;
	MarControlPtr isEmptyPtr;
	bool isEmptyState;

// constructor, destructor
	int method;
	MarSystem* makeSourceNet(std::string filename);
	MarSystem* makePitchNet(mrs_real source_osrate);
	MarSystem* makeAmplitudeNet(mrs_real source_osrate);
	void setupAllNet();
	void delNet();

// for playback
	bool hasAudio;



	realvec pitchList;
	realvec ampList;
// pitch stuff
	realvec pitchOld;
	realvec pitchMidiList;
    realvec amplitudeOld;

// transcribed stuff
	realvec durations;
	realvec notes;

// not used
 	MarControlPtr filenamePtr;
};
#endif

