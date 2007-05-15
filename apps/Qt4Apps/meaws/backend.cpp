// C++ testing
#include <iostream>
using namespace std;

#include "backend.h"
//using namespace Marsyas;

MarBackend::MarBackend(int getType) {
	method=getType;
	if (method==TYPE_INTONATION) {
		cout<<"Setting up Intonation stuff"<<endl;
		//startIntonation();
	}
	if (method==TYPE_CONTROL) {
		cout<<"Setting up Control stuff"<<endl;
		//startControl();
// currently no difference.
		//startIntonation();
	}
}

MarBackend::~MarBackend() {
/*
	if (method==TYPE_INTONATION) {
		delete mrsWrapper;
		delete recNet;
	}
	if (method==TYPE_CONTROL) {
	}
*/
}

void MarBackend::makeRecNet() {
/*
  MarSystemManager mng;

  recNet = mng.create("Series", "recNet");
  recNet->addMarSystem(mng.create("AudioSource", "srcRec"));
//	recNet->addMarSystem(mng.create("SoundFileSink","destRec")); // temporary

	recNet->updctrl("AudioSource/srcRec/mrs_real/israte", 44100.0);
  recNet->updctrl("AudioSource/srcRec/mrs_bool/initAudio", true);
*/
}

void MarBackend::setFileName(string filename) {
//	mrsWrapper->updctrl(filenamePtr, filename);
}

void MarBackend::startIntonation() {
/*
	makeRecNet();

	mrsWrapper = new MarSystemQtWrapper(recNet);
	mrsWrapper->start();
	filenamePtr = mrsWrapper->getctrl("SoundFileSink/destRec/mrs_string/filename");
	mrsWrapper->pause();
*/
}

void MarBackend::startControl() {

}

void MarBackend::playFile() {
	cout<<"playing file now.   (not really, but it will in a few days)"<<endl;
}

void MarBackend::start() {
	cout<<"play"<<endl;
//	mrsWrapper->play();
}

void MarBackend::stop() {
	cout<<"stop"<<endl;
//	mrsWrapper->pause();
}

