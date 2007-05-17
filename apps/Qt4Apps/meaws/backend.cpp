// C++ testing
#include <iostream>
using namespace std;

#include "backend.h"
//using namespace Marsyas;

MarBackend::MarBackend(int getType) {
	MarSystemManager mng;
	mrsWrapper = NULL;
	sourceNet = NULL;
	pitchNet = NULL;
	amplitudeNet = NULL;
	allNet = NULL;

	method=getType;
	if (method==TYPE_INTONATION) {
		cout<<"Setting up Intonation stuff"<<endl;
		startIntonation();
	}
	if (method==TYPE_CONTROL) {
		cout<<"Setting up Control stuff"<<endl;
		//startControl();
// currently no difference.
		//startIntonation();
	}
}

MarBackend::~MarBackend() {
	if (method==TYPE_INTONATION) {
		delNet();
	}
	if (method==TYPE_CONTROL) {
	}
}

void MarBackend::delNet() {
	cout<<"started delNet()"<<endl;
	mrsWrapper->pause();
	if (mrsWrapper != NULL) delete mrsWrapper;
	cout<<"del Wrap"<<endl;
	if (allNet != NULL) delete allNet;
	cout<<"del all"<<endl;
/*
	if (pitchNet != NULL) delete pitchNet;
	cout<<"del pitch"<<endl;
	if (amplitudeNet != NULL) delete amplitudeNet;
	cout<<"del ampl"<<endl;
	if (sourceNet != NULL) delete sourceNet;
*/
	cout<<"endded delNet()"<<endl;
}

MarSystem* MarBackend::makeSourceNet(std::string filename) {
	MarSystem *pnet = mng.create("Series", "sourceNet");
	if (filename != "") {
		pnet->addMarSystem(mng.create("SoundFileSource", "srcFile"));
		pnet->updctrl("SoundFileSource/srcFile/mrs_string/filename", filename);

		pnet->linkctrl("mrs_real/osrate", "SoundFileSource/srcFile/mrs_real/osrate");
	} else {
		pnet->addMarSystem(mng.create("AudioSource", "srcRec"));
		pnet->updctrl("AudioSource/srcRec/mrs_real/israte", 44100.0);
		pnet->updctrl("AudioSource/srcRec/mrs_bool/initAudio", true);

		pnet->linkctrl("mrs_real/osrate", "AudioSource/srcRec/mrs_real/osrate");
	}
	return pnet;
}

MarSystem* MarBackend::makePitchNet(mrs_real source_osrate) {
	MarSystem *pnet = mng.create("Series", "pitchNet");
	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
	pnet->addMarSystem(mng.create("PitchPraat", "pitch")); 
	pnet->addMarSystem(mng.create("RealvecSink", "rvSink")); 

	mrs_real lowPitch = 36;
	mrs_real highPitch = 79;
	mrs_real lowFreq = pitch2hertz(lowPitch);
	mrs_real highFreq = pitch2hertz(highPitch);

	mrs_natural lowSamples = hertz2samples(highFreq, source_osrate);
 	mrs_natural highSamples = hertz2samples(lowFreq, source_osrate);

	pnet->updctrl("PitchPraat/pitch/mrs_natural/lowSamples", lowSamples);
	pnet->updctrl("PitchPraat/pitch/mrs_natural/highSamples", highSamples);

	/*
	The window should be just long
	enough to contain three periods (for pitch detection) 
	of MinimumPitch. E.g. if MinimumPitch is 75 Hz, the window length
	is 40 ms and padded with zeros to reach a power of two.
	*/
	mrs_real windowSize = 3/lowPitch*source_osrate;
	pnet->updctrl("mrs_natural/inSamples", 512);
	pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", powerOfTwo(windowSize));

	return pnet;
}


void MarBackend::open(std::string filename) {
	delNet();
	cout<<"deleted"<<endl;
	sourceNet = makeSourceNet(filename);
	cout<<"created"<<endl;
	setupAllNet();
	cout<<"all created"<<endl;
}

void MarBackend::setFileName(string filename) {
	mrsWrapper->updctrl(filenamePtr, filename);
}

void MarBackend::startIntonation() {
	sourceNet = makeSourceNet("");
	setupAllNet();
}

void MarBackend::setupAllNet() {
	mrs_real osrate = sourceNet->getctrl("mrs_real/osrate")->toReal();
//	pitchNet = makePitchNet(osrate);

	allNet = mng.create("Series", "allNet");
	allNet->addMarSystem(sourceNet);
// test
//	allNet->addMarSystem(mng.create("AudioSink", "audioDest"));
//	allNet->updctrl("AudioSink/audioDest/mrs_bool/initAudio", true);
// non-test
	allNet->addMarSystem( makePitchNet(osrate) );
//	allNet->addMarSystem(pitchNet);

	mrsWrapper = new MarSystemQtWrapper(allNet);
	mrsWrapper->start();
//	mrsWrapper->pause();
}

void MarBackend::startControl() {

}

void MarBackend::playFile() {
	cout<<"playing file now.   (not really, but it will in a few days)"<<endl;
}

void MarBackend::start() {
	cout<<"play"<<endl;
	mrsWrapper->play();
}

void MarBackend::stop() {
	cout<<"stop"<<endl;
	mrsWrapper->pause();
}

void MarBackend::analyze() {
	delete mrsWrapper;

	mrs_real osrate = sourceNet->getctrl("mrs_real/osrate")->toReal();
	cout<<osrate<<endl;

	realvec data = allNet->getctrl("Series/pitchNet/RealvecSink/rvSink/mrs_realvec/data")->toVec();
	for (mrs_natural i=1; i<data.getSize();i+=2)
		data(i) = samples2hertz(data(i), osrate);
	allNet->updctrl("Series/pitchNet/RealvecSink/rvSink/mrs_bool/done", true); 


// my addition to the marsyasTest pitch stuff:
	int numPitches = data.getSize()/2;
	pitchList.create(numPitches); // fills with 0s as well
	for (int i=0; i<numPitches; i++) {
//		extra check is for low-/big-endian chips
		if (( data(2*i+1)>0 ) && (data(2*i+1)<10000)) {
			pitchList(i) = hertz2pitch( data(2*i+1) );
		}
	}
	cout<<pitchList;
//	pitchList.writeText("pitchList.txt");
}


