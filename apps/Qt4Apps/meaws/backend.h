#ifndef MEAWS_BACKEND_H
#define MEAWS_BACKEND_H

#include "defs.h"
#include <QObject>
// #include "MarSystemManager.h"
// #include "../MarSystemQtWrapper.h"

// using namespace Marsyas;

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
/*
signals:
	void nextNoteError(float error, int direction);
*/
private:
//	MarSystemQtWrapper *mrsWrapper;

	int method;
	int introBeats;

// "constructor"
	void makeRecNet();
	void startIntonation();
	void startControl();
/*
// metronome stuff
  MarSystem *metroNet;
	MarSystem *recNet;
	MarSystem *pitchNet;

	MarSystem *allNet;
  MarControlPtr filenamePtr;
	int *pitchError;

// pitch extracting stuff  (INCOMPLETE)
	void startPitchNet(string sfName);
	void stopPitchNet();
	MarSystem *pitchExtractor;
*/
};
#endif

