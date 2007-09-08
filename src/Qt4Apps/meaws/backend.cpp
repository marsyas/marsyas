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
	
	// does not compile if inside the switch ?!?
	MarSystem *fanout = mng.create("Fanout", "fanout");

	switch (method) {
	case TYPE_PLAYBACK:
		allNet->addMarSystem(mng.create("AudioSink", "audioDest"));
		allNet->updctrl("AudioSink/audioDest/mrs_bool/initAudio", true);
		break;
	case TYPE_INTONATION:
		allNet->addMarSystem( makePitchNet(osrate) );
		break;
	case TYPE_CONTROL:
		fanout->addMarSystem(makePitchNet(osrate));
		fanout->addMarSystem(makeAmplitudeNet(osrate));
		allNet->addMarSystem(fanout);
		break;
	case TYPE_SHIFT:
		allNet->addMarSystem( makePitchNet(osrate) );
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


MarSystem* MarBackend::makePitchNet(mrs_real source_osrate) {
	MarSystem *pnet = mng.create("Series", "pitchNet");
	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
	pnet->addMarSystem(mng.create("PitchPraat", "pitch")); 
	
	pitchesSink = mng.create("RealvecSink", "rvSink");
	pnet->addMarSystem(pitchesSink); 

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

MarSystem* MarBackend::makeAmplitudeNet(mrs_real source_osrate) {
	MarSystem *pnet = mng.create("Series", "amplitudeNet");
    // should be done with the source
	pnet->addMarSystem(mng.create("ShiftInput", "sfiAmp"));
	pnet->addMarSystem(mng.create("Power", "power")); 
	
	amplitudesSink = mng.create("RealvecSink", "amplitudeData");
	pnet->addMarSystem(amplitudesSink); 
    pnet->addMarSystem(mng.create("FlowCutSource", "fcs")); 
	
	pnet->updctrl("mrs_natural/inSamples", 512);
	pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", 512);
    pnet->updctrl("FlowCutSource/fcs/mrs_natural/setSamples", 2);
    pnet->updctrl("FlowCutSource/fcs/mrs_natural/setObservations", 1);

	return pnet;
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
		switch (method) {
		case TYPE_PLAYBACK:
			break;
		case TYPE_INTONATION: {
			getMidiPitches();
/*
			Transcriber *trans = new Transcriber();
			trans->setPitchList( getMidiPitches() );
			trans->calcOnsets();
			trans->calcNotes();
			trans->calcRelativeDurations();
			durations = trans->getDurations();
			notes = trans->getNotes();
			delete trans;
*/
			break;
		}
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
	// if we haven't calculated them already, do it now
	if (pitchList.getSize()==0) {
		mrs_real osrate =
sourceNet->getctrl("mrs_real/osrate")->to<mrs_real>();

		realvec data =
pitchesSink->getctrl("mrs_realvec/data")->to<realvec>();
		for (mrs_natural i=1; i<data.getSize();i+=2)
			data(i) = samples2hertz(data(i), osrate);
		pitchesSink->updctrl("mrs_bool/done", true); 

// my addition to the marsyasTest pitch stuff:
		int numPitches = data.getSize()/2;
		pitchList.create(numPitches); // fills with 0s as well
		for (int i=0; i<numPitches; i++) {
			// extra check is for low-/big-endian chips
			if (( data(2*i+1)>0 ) && (data(2*i+1)<10000)) {
				// the conversion should be done afterwrads hertz2pitch(
				pitchList(i) =  data(2*i+1);
			}
		}
	}
	
	return pitchList;
}

realvec MarBackend::getMidiPitches() {
	if (pitchList.getSize()==0) {
		getPitches();
	}
	pitchMidiList = getPitches();
	for (int i=0; i<pitchMidiList.getSize(); i++) {
		pitchMidiList(i) = hertz2pitch( pitchMidiList(i) );
		if (pitchMidiList(i) < 10)
			pitchMidiList(i) = 0;
	}
	return pitchMidiList;
}

realvec MarBackend::getAmplitudes() {
	if(amplitudeList.getSize()==0)
	{
	amplitudeList =
amplitudesSink->getctrl("mrs_realvec/data")->to<realvec>();
    amplitudesSink->updctrl("mrs_bool/done", true); 
	}
	
	return amplitudeList;
}

realvec MarBackend::getDurations()
{
	return durations;
}

realvec MarBackend::getNotes()
{
	return notes;
}


