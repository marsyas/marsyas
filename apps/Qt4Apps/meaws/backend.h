#ifndef MEAWS_BACKEND_H
#define MEAWS_BACKEND_H

#include "defs.h"
#include <QObject>
#include "MarSystemManager.h"
#include "../MarSystemQtWrapper.h"

using namespace Marsyas;

class MarBackend: public QObject {
	Q_OBJECT

public:
	MarBackend(int getType);
	~MarBackend();
//	void setIntro(int beats);
//	void playBeat();
	void start();
	void stop();
	void setFileName(std::string filename);
	void playFile();
	void open(std::string filename);
/*
signals:
	void nextNoteError(float error, int direction);
*/
private:
	MarSystemQtWrapper *mrsWrapper;
	MarSystemManager mng;
	MarSystem *sourceNet;
	MarSystem *pitchNet;
	MarSystem *amplitudeNet;
	MarSystem *allNet;

	realvec pitchList;
	int method;
	int introBeats;

// "constructor"
//	MarSystem* makeSourceNet(std::string filename);
	MarSystem* makePitchNet(std::string filename);
	MarSystem* makeAmplitudeNet();

//	void makeRecNet();
	void startIntonation();
	void startControl();
  MarControlPtr filenamePtr;
/*
// metronome stuff
  MarSystem *metroNet;
	MarSystem *pitchNet;

	MarSystem *allNet;
	int *pitchError;

// pitch extracting stuff  (INCOMPLETE)
	void startPitchNet(string sfName);
	void stopPitchNet();
	MarSystem *pitchExtractor;
*/
};
#endif

