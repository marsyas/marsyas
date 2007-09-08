#include "backend.h"

/*   ***************************
 *   ***                     ***
 *   *** Basic program stuff ***
 *   ***                     ***
 *   ***************************
 */

MarBackend::MarBackend(int getType) {
	method=getType;

	mrsWrapper = NULL;
	allNet = NULL;
	sourceNet = makeSourceNet("");

	pitchList = NULL;
	ampList = NULL;
	isEmptyState = false;
	hasAudio = false;

	setupAllNet();
}

MarBackend::~MarBackend() {
	delNet();
}

void MarBackend::delNet() {
	if (mrsWrapper != NULL) {
		mrsWrapper->pause();
		delete mrsWrapper;
		mrsWrapper = NULL;
	}
	if (allNet != NULL) {
		delete allNet;
		allNet = NULL;
		sourceNet = NULL;
		hasAudio = false;
	} else {
		if (sourceNet != NULL) {
			delete sourceNet;
			sourceNet = NULL;
			hasAudio = false;
		}
	}
}

MarSystem* MarBackend::makeSourceNet(std::string filename) {
	MarSystem *pnet = mng.create("Series", "sourceNet");
	if (filename != "") {
		MRSERR("DEBUG: getting from file");
		pnet->addMarSystem(mng.create("SoundFileSource", "srcFile"));
		pnet->updctrl("SoundFileSource/srcFile/mrs_string/filename", filename);
		pnet->linkctrl("mrs_real/osrate", "SoundFileSource/srcFile/mrs_real/osrate");
		pnet->linkctrl("mrs_bool/notEmpty", "SoundFileSource/srcFile/mrs_bool/notEmpty");
	} else {
		MRSERR("DEBUG: getting audio");
		pnet->addMarSystem(mng.create("AudioSource", "srcRec"));
		pnet->updctrl("mrs_real/israte", 44100.0);
		pnet->updctrl("AudioSource/srcRec/mrs_bool/initAudio", true);
		pnet->linkctrl("mrs_bool/notEmpty", "AudioSource/srcRec/mrs_bool/notEmpty");
		//pnet->linkctrl("mrs_real/osrate", "AudioSource/srcRec/mrs_real/osrate");
	}
	return pnet;
}


void MarBackend::open(std::string filename) {
	delNet();
	sourceNet = makeSourceNet(filename);
	hasAudio = true;
	setupAllNet();
	mrsWrapper->play();
}

/*
void MarBackend::setFileName(string filename) {
	mrsWrapper->updctrl(filenamePtr, filename);
}
*/

void MarBackend::playFile() {
	if (hasAudio) {
		string filename =
sourceNet->getctrl("SoundFileSource/srcFile/mrs_string/filename")->to<mrs_string>();
		delNet();
		sourceNet = makeSourceNet(filename);
		method = TYPE_PLAYBACK;
		setupAllNet();
	}
}

void MarBackend::ctrlChanged(MarControlPtr changed) {
	if ( changed.isEqual( isEmptyPtr ) ) {
		bool changedState = changed->to<mrs_bool>();
		if (changedState != isEmptyState) {
			if ( changedState ) {
				//start();
				isEmptyState=true;
			} else {
				stop();
				isEmptyState=false;
			}
		}
	}
}

void MarBackend::start() {
	emit setAttempt(true);
	if (mrsWrapper != NULL)
		mrsWrapper->play();
}

void MarBackend::stop() {
	emit setAttempt(false);
	if (mrsWrapper != NULL)
		mrsWrapper->pause();
	hasAudio = true;
}

mrs_real MarBackend::getRate() {
	if (sourceNet != NULL)
		return sourceNet->getctrl("mrs_real/osrate")->to<mrs_real>();
	else
		return 0;
}

/*   ***************************
 *   ***                     ***
 *   *** Test-specific stuff ***
 *   ***   (before  audio)   ***
 *   ***                     ***
 *   ***************************
 */

void MarBackend::setupAllNet() {
	mrs_real osrate =
sourceNet->getctrl("mrs_real/osrate")->to<mrs_real>();
	allNet = mng.create("Series", "allNet");
	allNet->addMarSystem(sourceNet);
	allNet->linkctrl("mrs_bool/notEmpty", "Series/sourceNet/mrs_bool/notEmpty");
	
	pitchSink = mng.create("RealvecSink", "rvSink");
	ampSink = mng.create("RealvecSink", "amplitudeData");

	switch (method) {
	case TYPE_PLAYBACK:
		allNet->addMarSystem(mng.create("AudioSink", "audioDest"));
		allNet->updctrl("AudioSink/audioDest/mrs_bool/initAudio", true);
		break;
	case TYPE_INTONATION:
	{
		MarSystem *fanout = mng.create("Fanout", "fanout");
		fanout->addMarSystem( TranscriberExtract::makePitchNet(osrate, 200.0, pitchSink));
		fanout->addMarSystem(
TranscriberExtract::makeAmplitudeNet( ampSink ));
		allNet->addMarSystem(fanout);
		break;
	}
	case TYPE_CONTROL:
	{
/*
	MarSystem *fanout = mng.create("Fanout", "fanout");
		fanout->addMarSystem(makePitchNet(osrate));
		fanout->addMarSystem(makeAmplitudeNet(osrate));
		allNet->addMarSystem(fanout);
*/
		break;
	}
	case TYPE_SHIFT:
//		allNet->addMarSystem( makePitchNet(osrate) );
		break;
	default:
		MRSERR("backend var method is broken");
		break;
	}

	mrsWrapper = new MarSystemQtWrapper(allNet);
	isEmptyPtr = mrsWrapper->getctrl("mrs_bool/notEmpty");
	mrsWrapper->trackctrl( isEmptyPtr );
	connect(mrsWrapper, SIGNAL(ctrlChanged(MarControlPtr)), this, SLOT(ctrlChanged(MarControlPtr)));

	mrsWrapper->start();
	mrsWrapper->pause();
//	emit setAttempt(false);
}


/*   ***************************
 *   ***                     ***
 *   *** Test-specific stuff ***
 *   ***    (after audio)    ***
 *   ***                     ***
 *   ***************************
 */

bool MarBackend::analyze() {
	if (mrsWrapper != NULL) {
		delete mrsWrapper;
		mrsWrapper = NULL;
	}
	if (allNet != NULL) {
		mrs_real osrate =
sourceNet->getctrl("mrs_real/osrate")->to<mrs_real>();
		switch (method) {
		case TYPE_PLAYBACK:
			break;
		case TYPE_INTONATION:
			pitchList = TranscriberExtract::getPitchesFromRealvecSink(pitchSink, osrate);
			ampList =
TranscriberExtract::getAmpsFromRealvecSink(ampSink);
			break;
		case TYPE_CONTROL:
			getPitches();
			getAmplitudes();
			break;
		case TYPE_SHIFT:
			getMidiPitches();
			break;
		default:
			MRSERR("backend var method is broken 2");
			break;
		}
		return true;
	}
	return false;
}

realvec MarBackend::getPitches() {
	if (pitchList != NULL)
		return (*pitchList);
	else
		return NULL;
}

realvec MarBackend::getMidiPitches() {
	if (pitchList != NULL)
	{
		TranscriberExtract::toMidi( pitchList );
		return (*pitchList);
	}
	else
		return NULL;
}

realvec MarBackend::getAmplitudes() {
	if (ampList != NULL)
		return (*ampList);
	else
		return NULL;
}

/*
realvec MarBackend::getDurations()
{
	return durations;
}

realvec MarBackend::getNotes()
{
	return notes;
}
*/

