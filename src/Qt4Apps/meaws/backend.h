#ifndef MEAWS_BACKEND_H
#define MEAWS_BACKEND_H

#include "defs.h"
#include <QObject>
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
//#include "Transcriber.h"
using namespace Marsyas;
using namespace MarsyasQt;

class MarBackend: public QObject {
	Q_OBJECT

public:
	MarBackend(int getType);
	~MarBackend();

//	void setFileName(std::string filename);
	void playFile();
	void open(std::string filename);

	bool analyze();
	realvec getPitches();
	realvec getMidiPitches();
	realvec getAmplitudes();

// transcriber
	realvec getDurations();
	realvec getNotes();

	mrs_real getRate();

public slots:
	void ctrlChanged(MarControlPtr changed);
	void start();
	void stop();

signals:
	void setAttempt(bool running);

private:
	MarSystemManager mng;
	MarSystem *sourceNet;
	MarSystem *allNet;
	MarSystem *amplitudesSink;
	MarSystem *pitchesSink;
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

// pitch stuff
	realvec pitchList;
	realvec pitchMidiList;
    realvec amplitudeList;

// transcribed stuff
	realvec durations;
	realvec notes;

// not used
 	MarControlPtr filenamePtr;
};
#endif

