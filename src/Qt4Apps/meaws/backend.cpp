#include "backend.h"

/*   ***************************
 *   ***                     ***
 *   *** Basic program stuff ***
 *   ***                     ***
 *   ***************************
 */

MarBackend::MarBackend() {
	mrsWrapper = NULL;
	allNet = NULL;
	sourceNet = NULL;
	method_ = -1;
	filename_ = "";
	setupChanged_ = true;
	hasAudio_ = false;
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
//		hasAudio = false;
	} else {
		if (sourceNet != NULL) {
			delete sourceNet;
			sourceNet = NULL;
//			hasAudio = false;
		}
	}
}

MarSystem* MarBackend::makeSourceNet(bool fromFile) {
	cout<<"makeSourceNet"<<endl;
	cout<<"method: "<<method_<<" fromFile? "<<fromFile<<endl;
	MarSystem *pnet = mng.create("Series", "sourceNet");
	if (fromFile) {
		MRSERR("DEBUG: getting from file");
		pnet->addMarSystem(mng.create("SoundFileSource", "srcFile"));
		pnet->updctrl("SoundFileSource/srcFile/mrs_string/filename",
			filename_);
		pnet->linkctrl("mrs_real/osrate",
			"SoundFileSource/srcFile/mrs_real/osrate");
		pnet->linkctrl("mrs_bool/notEmpty",
			"SoundFileSource/srcFile/mrs_bool/notEmpty");
//		pnet->linkctrl("mrs_string/filename",
//			"SoundFileSource/srcFile/mrs_string/filename");
	} else {
		MRSERR("DEBUG: getting audio");
		pnet->addMarSystem(mng.create("AudioSource", "srcRec"));
		pnet->updctrl("mrs_real/israte", 44100.0);
		pnet->updctrl("AudioSource/srcRec/mrs_bool/initAudio", true);
		pnet->linkctrl("mrs_bool/notEmpty",
			"AudioSource/srcRec/mrs_bool/notEmpty");
		pnet->addMarSystem(mng.create("SoundFileSink", "saveFile"));
		pnet->updctrl("SoundFileSink/saveFile/mrs_string/filename",
			filename_);
//		pnet->linkctrl("mrs_string/filename",
//			"SoundFileSource/saveFile/mrs_string/filename");
		//pnet->linkctrl("mrs_real/osrate",
		//	"AudioSource/srcRec/mrs_real/osrate");
	}
	return pnet;
}

/*   ***************************
 *   ***                    ***
 *   *** Communication with ***
 *   *** the rest  of MEAWS ***
 *   ***                    ***
 *   ***************************
 */

void MarBackend::setBackend(mrs_natural method, mrs_bool hasAudio,
		std::string filename)
{
	method_ = method;
	hasAudio_ = hasAudio;
	filename_ = filename;
	setupChanged_ = true;
}


void MarBackend::setup() {
	cout<<"setup"<<endl;
//	cout<<"  hasaudio: "<<hasAudio<<endl;
	delNet();
	sourceNet = makeSourceNet(hasAudio_);
	setupAllNet();
	isEmptyState = false;
	setupChanged_ = false;

	cout<<"done setup"<<endl;
}

void MarBackend::ctrlChanged(MarControlPtr changed) {
	if ( changed.isEqual( isEmptyPtr ) ) {
		bool changedState = changed->to<mrs_bool>();
		if (changedState != isEmptyState) {
			if ( changedState ) {
				//start();
				isEmptyState=true;
			} else {
//				stop();
				emit setAttempt(false);
				isEmptyState=false;
			}
		}
	}
}

void MarBackend::start() {
	cout<<"start"<<endl;
	if (setupChanged_) {
		cout<<"should not happen!"<<endl;
		setup();
	}
	emit setAttempt(true);
	if (mrsWrapper != NULL) {
		mrsWrapper->play();
	} else {
		cout<<"REALLY should not happen"<<endl;
	}
}

void MarBackend::stop() {
	emit setAttempt(false);
	if (mrsWrapper != NULL)
		mrsWrapper->pause();
	hasAudio_ = true;
	cout<<"stopped"<<endl;
	emit gotAudio();
	method_ = BACKEND_PLAYBACK;
	setupChanged_ = true;
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
	cout<<"setupAllNet"<<endl;
	cout<<"\t "<<method_<<" "<<hasAudio_<<endl;

	mrs_real osrate =
	  sourceNet->getctrl("mrs_real/osrate")->to<mrs_real>();
	allNet = mng.create("Series", "allNet");
	allNet->addMarSystem(sourceNet);
	allNet->linkctrl("mrs_bool/notEmpty", "Series/sourceNet/mrs_bool/notEmpty");
	
	pitchSink = mng.create("RealvecSink", "rvSink");
	ampSink = mng.create("RealvecSink", "amplitudeData");

//zz
	MarSystem *fanout = mng.create("Fanout", "fanout");

	switch (method_) {
	case (BACKEND_PLAYBACK):
		fanout->addMarSystem(mng.create("AudioSink", "audioDest"));
		fanout->updctrl("mrs_real/israte", osrate);
		fanout->updctrl("AudioSink/audioDest/mrs_bool/initAudio", true);
		break;
	case (BACKEND_PITCHES): {
		fanout->addMarSystem( TranscriberExtract::makePitchNet(
		  osrate, 200.0, pitchSink));
		break;
	}
	case (BACKEND_AMPLITUDES): {
		fanout->addMarSystem(
		  TranscriberExtract::makeAmplitudeNet( ampSink ));
		break;
	}
	case (BACKEND_PITCHES_AMPLITUDES): {
		fanout->addMarSystem( TranscriberExtract::makePitchNet(
		  osrate, 200.0, pitchSink));
		fanout->addMarSystem(
		  TranscriberExtract::makeAmplitudeNet( ampSink ));
		break;
	}
//	case TYPE_CONTROL:
//	{
/*
	MarSystem *fanout = mng.create("Fanout", "fanout");
		fanout->addMarSystem(makePitchNet(osrate));
		fanout->addMarSystem(makeAmplitudeNet(osrate));
		allNet->addMarSystem(fanout);
*/
//		break;
//	}
//	case TYPE_SHIFT:
//		allNet->addMarSystem( makePitchNet(osrate) );
//		break;
	default:
		MRSERR("backend var method is broken");
		break;
	}
	allNet->addMarSystem(fanout);

	mrsWrapper = new MarSystemQtWrapper(allNet);
	isEmptyPtr = mrsWrapper->getctrl("mrs_bool/notEmpty");

//zz
/*
	if (method_ == BACKEND_PLAYBACK)
		filenamePtr = mrsWrapper->getctrl(
			"Series/sourceNet/mrs_string/filename");
	else
		filenamePtr = mrsWrapper->getctrl(
			"Series/sourceNet/mrs_string/filename");
*/

	mrsWrapper->trackctrl( isEmptyPtr );
	connect(mrsWrapper, SIGNAL(ctrlChanged(MarControlPtr)),
	  this, SLOT(ctrlChanged(MarControlPtr)));

	mrsWrapper->start();
	mrsWrapper->pause();
	ctrlChanged(isEmptyPtr);
//	emit setAttempt(false);
	cout<<"done setupallnet"<<endl;
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
		switch (method_) {
		case BACKEND_PLAYBACK:
			break;
		case BACKEND_PITCHES:
			pitchList = TranscriberExtract::getPitchesFromRealvecSink(pitchSink, osrate);
			break;
		case BACKEND_AMPLITUDES:
			ampList = TranscriberExtract::getAmpsFromRealvecSink(ampSink);
			break;
		case BACKEND_PITCHES_AMPLITUDES:
			pitchList = TranscriberExtract::getPitchesFromRealvecSink(pitchSink, osrate);
			ampList = TranscriberExtract::getAmpsFromRealvecSink(ampSink);
			break;
/*
		case TYPE_CONTROL:
			getPitches();
			getAmplitudes();
			break;
		case TYPE_SHIFT:
			getMidiPitches();
			break;
*/
		default:
			MRSERR("backend var method is broken 2");
			break;
		}
		return true;
	}
	return false;
}

realvec MarBackend::getPitches() {
	return pitchList;
}

realvec MarBackend::getMidiPitches() {
	TranscriberExtract::toMidi( pitchList );
	return pitchList;
}

realvec MarBackend::getAmplitudes() {
	return ampList;
}

