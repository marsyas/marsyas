// currently refactoring this file.  -gp
// functions are uncommented as they are certified for correctness and
// lack of compiler warnings.


/**
	\class Transcriber
	\ingroup Notmar
	\brief A simple pitch-based music transcription object

	Usage:
	- setPitchList() or getPitchesFromAudio()
	- calcOnsets()
	- calcNotes()
	- (optional)  calcRelativeDurations().  MUST BE AFTER THE FIRST TWO!
	- to see the results, use getOnsets() and getNotes(), and/or
	  getDurations()
*/


#include "Transcriber.h"
static MarSystemManager mng;

#include <iostream>
//#include <fstream>
//#include <cmath>

Transcriber::Transcriber() {
    /*
        	median_radius = 10;
        	new_note_midi = 0.6;
        	pitch_certainty_div = 500;

        	// tries to avoid octave errors
        	LOWEST_NOTE = 40;
        	HIGHEST_NOTE = 90;
    */
}

Transcriber::~Transcriber() {
    /*
        	pitchList.~realvec();
        	notes.~realvec();
    */
}

#define EMPTYSTRING "MARSYAS_EMPTY"

// TODO: ask -devel about making this a general Marsyas function
void
Transcriber::addFileSource(MarSystem* net, string infile, mrs_real &srate)
{
	if (infile == EMPTYSTRING) {
		// TODO: spend 1 hour and finally figure out WTF the MRS_FOO
		// things do.
		cout << "Please specify a sound file." << endl; exit(1); }

    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->updctrl("SoundFileSource/src/mrs_string/filename", infile);
    srate =
        net->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
    net->linkControl("mrs_bool/notEmpty",
                     "SoundFileSource/src/mrs_bool/notEmpty");
}

/*
MarSystem*
Transcriber::makePitchNet(MarSystemManager mng, mrs_real source_osrate, mrs_real lowFreq)
{
	cout<<"making pitch"<<endl;
	mng.create("Parallel", "para");
    //MarSystem *net = mng.create("Series", "pitchNet");
cout<<"made series"<<endl;
    //net->addMarSystem(mng.create("ShiftInput", "sfi"));
    //net->addMarSystem(mng.create("PitchPraat", "pitch"));
cout<<"made network"<<endl;
//   pitchesSink = mng.create("RealvecSink", "rvSink");
//    net->addMarSystem(pitchesSink);
//    net->addMarSystem(mng.create("Gain", "gain"));
//   mrs_real lowPitch = 36;
//   mrs_real highPitch = 100;
    //mrs_real lowFreq = pitch2hertz(lowPitch);
    mrs_real highFreq = 5000.0;
*/
/*
    mrs_natural lowSamples = hertz2samples(highFreq, source_osrate);
    mrs_natural highSamples = hertz2samples(lowFreq, source_osrate);

    net->updctrl("PitchPraat/pitch/mrs_natural/lowSamples",
                 lowSamples);
    net->updctrl("PitchPraat/pitch/mrs_natural/highSamples",
                 highSamples);
    //The window should be just long enough to contain three periods (for
    //pitch detection) of MinimumPitch.
cout<<"window"<<endl;
    mrs_real windowSize = 3/lowFreq*source_osrate;
    net->updctrl("mrs_natural/inSamples", 512);
    net->updctrl("ShiftInput/sfi/mrs_natural/WindowSize",
                 powerOfTwo(windowSize));

*/
//    return NULL;
//}

realvec Transcriber::getPitchesFromAudio(string audioFilename) {
    realvec pitchList;
    mrs_real srate;

	MarSystem* pnet = mng.create("Series", "pnet");
	addFileSource(pnet, audioFilename, srate);
//	addFileSource(mng, pnet, audioFilename, srate);
/*
	pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->updctrl("SoundFileSource/src/mrs_string/filename",
audioFilename);
    srate =
        pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
    pnet->linkControl("mrs_bool/notEmpty",
                     "SoundFileSource/src/mrs_bool/notEmpty");
*/
	pnet->addMarSystem(mng.create("AudioSink", "dest"));
	pnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	while (
pnet->getctrl("mrs_bool/notEmpty")->toBool()
//pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool()
)
	{
		pnet->tick();
	}
	delete pnet;
    return pitchList;
}

//    makePitchNet(mng, srate, 100.0);
    //pnet->addMarSystem( makePitchNet(mng, srate, 100.0) );
    /*

        // sets up SoundFileSource, links notEmpty, and sets srate
        pnet->addMarSystem(mng.create("SoundFileSource", "src"));
        pnet->updctrl("SoundFileSource/src/mrs_string/filename",
    audioFilename);
        mrs_real srate =
    		pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
        pnet->linkControl("mrs_bool/notEmpty",
                         "SoundFileSource/src/mrs_bool/notEmpty");
    */


    /*
        pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
        pnet->addMarSystem(mng.create("PitchPraat", "pitch"));
        pnet->addMarSystem(mng.create("RealvecSink", "rvSink"));

        mrs_real lowPitch = 50;
        mrs_real highPitch = 100;
        mrs_real lowFreq = pitch2hertz(lowPitch);
        mrs_real highFreq = pitch2hertz(highPitch);
        // note the reversed order!
        mrs_natural lowSamples = hertz2samples(highFreq, srate);
        mrs_natural highSamples = hertz2samples(lowFreq, srate);

        pnet->updctrl("PitchPraat/pitch/mrs_natural/lowSamples",
                      lowSamples);
        pnet->updctrl("PitchPraat/pitch/mrs_natural/highSamples",
                      highSamples);

        //  The window should be just long enough to contain three periods
        //  (for pitch detection) of MinimumPitch. E.g. if MinimumPitch is
        //  75 Hz, the window length is 40 ms and padded with zeros to reach
        //  a power of two.
        mrs_real windowSize = 3.0 / lowPitch * srate;
        pnet->updctrl("mrs_natural/inSamples", 512);
        pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize",
                      powerOfTwo(windowSize));
    */
/*
    while (pnet->getctrl("mrs_bool/notEmpty")->toBool())
        pnet->tick();

    realvec data =
        pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();
    for (mrs_natural i=1; i<data.getSize(); i+=2)
        data(i) = samples2hertz(data(i), srate);
    pnet->updctrl("RealvecSink/rvSink/mrs_bool/done", true);

    cout<<"Trans: got pitches"<<endl;
    mrs_real pitch;
    pitchList.create(data.getSize()/2);
    for (mrs_natural i=0; i<data.getSize(); i++) {
        pitch = data(2*i+1);
        if ( pitch>0 )
            pitchList(i)=pitch;
    }
    cout<<"Trans: done transfer"<<endl;
}
*/




/*
void
Transcriber::setOptions(mrs_natural getRadius, mrs_real getNewNote, mrs_real getCertantyDiv)
{
	median_radius = getRadius;
	new_note_midi = getNewNote;
	pitch_certainty_div = getCertantyDiv;
}

void
Transcriber::setPitchList(realvec newPitchList)
{
	pitchList = newPitchList;
}

realvec Transcriber::getAmplitudesFromAudio(string audioFilename) {
	MarSystemManager mng;

    MarSystem* pnet = mng.create("Series", "pnet");

    pnet->addMarSystem(mng.create("SoundFileSource", "src"));
    pnet->updctrl("SoundFileSource/src/mrs_string/filename",
audioFilename);
    mrs_real srate =
        pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
    pnet->linkControl("mrs_bool/notEmpty",
                     "SoundFileSource/src/mrs_bool/notEmpty");

    pnet->addMarSystem(mng.create("ShiftInput", "sfiAmp"));
    pnet->addMarSystem(mng.create("Power", "power"));

    pnet->addMarSystem(mng.create("RealvecSink", "rvSink"));

    pnet->updctrl("mrs_natural/inSamples", 512);
    pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", 512);
    pnet->updctrl("FlowCutSource/fcs/mrs_natural/setSamples", 2);
    pnet->updctrl("FlowCutSource/fcs/mrs_natural/setObservations", 1);


    while (pnet->getctrl("mrs_bool/notEmpty")->toBool())
        pnet->tick();

	realvec data =
pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();

	cout<<"Trans: got amplitudes"<<endl;
	mrs_real amp;
	ampList.create(data.getSize()/2);
	for (mrs_natural i=0; i<data.getSize(); i++) {
		amp = data(2*i+1);
		if ( amp>0 )
			ampList(i)=amp;
	}
	cout<<"Trans: done transfer"<<endl;
    delete pnet;
	return ampList;
}
//zz

realvec Transcriber::getNotes() {
	return notes;
}

realvec Transcriber::getOnsets() {
	return onsets;
}

realvec Transcriber::getDurations() {
	return durations;
}

// find median without 0s.
mrs_real Transcriber::findMedian(mrs_natural start, mrs_natural length, realvec array) {
	if ( length<=0 ) return 0;
	mrs_real toReturn;
	realvec myArray;
	myArray.allocate(length);
	mrs_natural j=0;
	for (mrs_natural i=0; i<length; i++) {
		// don't include 0s
		if ( array(start+i) > 0 ) {
			myArray(j)=array(start+i);
			j++;
		}
	}
	myArray.stretch(j-1);
	if (j-1 > 0)
		toReturn = myArray.median();
	else
		toReturn = 0;

	myArray.~realvec();
	return toReturn;
}

mrs_natural secToFrame(mrs_real second)
{
	//return (int) round( second*44100.0/512.0 ); //round() does not exist in <cmath> [!]
	return (mrs_natural) floor(0.5 + second*44100.0/512.0);

}

void Transcriber::setOnsets(string filename) {
	onsets.readText(filename);
	for (mrs_natural i=0; i<onsets.getSize(); i++) {
		onsets(i) = secToFrame( onsets(i) );
	}
}

void Transcriber::calcOnsets() {
	// temporary-ish, to work around a PPC bug in realvec stretch()
	// err, being 4 instead of 1 is the workaround.
	onsets.create(4);
	mrs_natural i;

	float median;
	float prevNote=0.0;
	mrs_natural durIndex=1;
	mrs_natural prevSamp=0;
	for (i=median_radius; i<pitchList.getSize()-median_radius; i++) {
		median = findMedian(i-median_radius, 2*median_radius, pitchList);
		if ( fabs(median-prevNote) > new_note_midi ) {
			if (i>prevSamp+median_radius) {
				prevNote = median;
				prevSamp = i;
				onsets.stretchWrite( durIndex, i);
				durIndex++;
			}
			else {
				prevNote = median;
				prevSamp = i;
				onsets(durIndex) = i;
			}
		}
	}
	onsets.stretchWrite(durIndex, pitchList.getSize() );
	onsets.stretch(durIndex+1);
}

void Transcriber::calcRelativeDurations() {
	durations.create( onsets.getSize()-1 );

	mrs_natural i;
	mrs_natural min = 99999; // infinity
	// calculate durations in samples
	// and find smallest
	for (i=0; i<onsets.getSize()-1; i++) {
		durations(i) = onsets(i+1) - onsets(i);
//		cout<<"duration: "<<durations(i)<<endl;
		// we don't care about silent durations
		if ((durations(i) < min) && (notes(i)>0))
			min = durations(i);
	}
//	cout<<"min: "<<min<<endl;
	// find relative durations
	// yes, we want to truncate the division.
	for (i=0; i<onsets.getSize()-1; i++) {
		durations(i) = (mrs_natural) ( durations(i) / (min*0.9) );
	}
}

void Transcriber::calcNotes(){
	notes.create( onsets.getSize()-1 );

	// first pass: median pitch
	mrs_natural start, end;
	mrs_real pitch;
	mrs_natural i;
	for (i=0; i<onsets.getSize()-1; i++) {
	    start = (mrs_natural) onsets(i);
		end = (mrs_natural) onsets(i+1);
		pitch = findMedian( start, end-start, pitchList);
		notes(i) = pitch;
	}

	// second pass: median of close pitches
	realvec closePitches;
	closePitches.create(1);
	mrs_real distance;
	mrs_natural j, k;
	for (i=0; i<onsets.getSize()-1; i++) {
	    start = (mrs_natural) onsets(i);
		end = (mrs_natural) onsets(i+1);
		closePitches.stretch(end-start);
		k=0;
		for (j=start; j<end; j++) {
			distance = fabs( pitchList(j) - notes(i) );
			if (distance < new_note_midi) {
				closePitches(k) = pitchList(j);
				k++;
			}
		}
		closePitches.stretch(k-1);
   		pitch = findMedian(0, closePitches.getSize(), closePitches);
//		pitch = round(pitch);
		notes(i) = pitch;
	}
}
*/

