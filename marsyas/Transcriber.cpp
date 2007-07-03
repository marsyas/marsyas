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


#include <iostream>
#include <fstream>
#include <cmath>
#include "Transcriber.h"

Transcriber::Transcriber() {
	median_radius = 10;
	new_note_midi = 0.6;
	pitch_certainty_div = 500;

	// tries to avoid octave errors
	LOWEST_NOTE = 40;
	HIGHEST_NOTE = 90;
}

Transcriber::~Transcriber() {
	pitchList.~realvec();
	notes.~realvec();
}

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

// I recommend doing the pitches yourself and pass them in
// via setPitchList(), and not using this function.
void Transcriber::getPitchesFromAudio(string audioFilename) {
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", audioFilename);
  pnet->addMarSystem(mng.create("PitchPraat", "pitch")); 
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink")); 

  mrs_real lowPitch = LOWEST_NOTE;
  mrs_real highPitch = HIGHEST_NOTE;
  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);

  mrs_natural lowSamples = 
     hertz2samples(highFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
  mrs_natural highSamples = 
     hertz2samples(lowFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
 
  pnet->updctrl("PitchPraat/pitch/mrs_natural/lowSamples", lowSamples);
  pnet->updctrl("PitchPraat/pitch/mrs_natural/highSamples", highSamples);
  
  //  The window should be just long
  //  enough to contain three periods (for pitch detection) 
  //  of MinimumPitch. E.g. if MinimumPitch is 75 Hz, the window length
  //  is 40 ms and padded with zeros to reach a power of two.
  mrs_real windowSize = 3/lowPitch*pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
  pnet->updctrl("mrs_natural/inSamples", 512);
	// pnet->updctrl("ShiftInput/sfi/mrs_natural/Decimation", 256);
	pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", powerOfTwo(windowSize));
	//pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", 1024);

  while (pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
   pnet->tick();

	realvec data = pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();
   for (mrs_natural i=1; i<data.getSize();i+=2)
	   data(i) = samples2hertz(data(i), pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());

   pnet->updctrl("RealvecSink/rvSink/mrs_bool/done", true); 


// my addition to the marsyasTest pitch stuff:
	mrs_natural i;
	mrs_real maxConf=0.0;
	for (i=0; i<data.getSize(); i=i+2) {
		if (maxConf < data(i))
			maxConf = data(i);
	}

	pitchList.allocate( data.getSize()/2 );
	for (i=0; i<pitchList.getSize(); i++) {
		if ( data(2*i+1) > 0 )
			if (data(2*i) > maxConf/pitch_certainty_div)
				pitchList(i) = hertz2pitch( data(2*i+1) );
			else
				pitchList(i) = 0.0;
		else
			pitchList(i) = 0.0;
	}

	delete pnet;
}

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

