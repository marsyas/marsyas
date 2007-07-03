#ifndef MEAWS_BACKEND_H
#define MEAWS_BACKEND_H

#include "defs.h"
#include <QObject>
#include "MarSystemManager.h"
#include "../MarSystemQtWrapper.h"
#include "Transcriber.h"
using namespace Marsyas;

class MarBackend: public QObject {
	Q_OBJECT

public:
	MarBackend(int getType);
	~MarBackend();

//	void setFileName(std::string filename);
	void playFile();
	void open(std::string filename);

	void analyze();
	realvec getPitches();

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
	MarSystemQtWrapper *mrsWrapper;
	MarControlPtr emptyPtr;

// constructor, destructor
	int method;
	MarSystem* makeSourceNet(std::string filename);
	MarSystem* makePitchNet(mrs_real source_osrate);
	void setupAllNet();
	void delNet();

// pitch stuff
	realvec pitchList;


	bool tempold;
// not used
 	MarControlPtr filenamePtr;
};
#endif

