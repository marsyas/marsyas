// currently refactoring this file.  -gp
// functions are uncommented as they are certified for correctness and
// lack of compiler warnings.


/**
	\class Transcriber
	\ingroup Notmar
	\brief A collection of functions which simplify transcription (pitch
extraction, amplitude extraction, etc).

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

Transcriber::Transcriber()
{
}

Transcriber::~Transcriber()
{
}

MarSystem*
Transcriber::makePitchNet(const mrs_real srate, const mrs_real lowFreq, MarSystem* rvSink)
{
	mrs_real highFreq = 5000.0;

	MarSystem *net = mng.create("Series", "pitchNet");
	net->addMarSystem(mng.create("ShiftInput", "sfi"));
	net->addMarSystem(mng.create("PitchPraat", "pitch"));
	if (rvSink != NULL)
		net->addMarSystem(rvSink);

	// yes, this is the right way around (lowSamples<-highFreq)
	net->updctrl("PitchPraat/pitch/mrs_natural/lowSamples",
	             hertz2samples(highFreq, srate) );
	net->updctrl("PitchPraat/pitch/mrs_natural/highSamples",
	             hertz2samples(lowFreq, srate) );

	// The window should be just long enough to contain three periods
	// (for pitch detection) of MinimumPitch.
	mrs_real windowSize = 3.0/lowFreq*srate;
	net->updctrl("mrs_natural/inSamples", 512);
	net->updctrl("ShiftInput/sfi/mrs_natural/WindowSize",
	             powerOfTwo(windowSize));

	return net;
}

MarSystem* Transcriber::makeAmplitudeNet(MarSystem* rvSink)
{
	MarSystem *net = mng.create("Series", "amplitudeNet");
	net->addMarSystem(mng.create("ShiftInput", "sfiAmp"));
	net->addMarSystem(mng.create("Rms", "rms"));
	if (rvSink != NULL)
		net->addMarSystem(rvSink);

	net->updctrl("mrs_natural/inSamples", 512);
	net->updctrl("ShiftInput/sfiAmp/mrs_natural/WindowSize", 512);

	return net;
}

void
Transcriber::getAllFromAudio(const string audioFilename, realvec* &
                             pitchList, realvec* &ampList, realvec* &
                             boundaries)
{
	MarSystem* pitchSink = mng.create("RealvecSink", "pitchSink");
	MarSystem* ampSink = mng.create("RealvecSink", "ampSink");

	MarSystem* pnet = mng.create("Series", "pnet");
	mrs_real srate = Easymar::addFileSource(pnet, audioFilename);
// TODO: double the number of observations?
//	pnet->addMarSystem(mng.create("ShiftInput", "shift"));
//   pnet->updctrl("ShiftInput/shift/mrs_natural/WindowSize",1024);
	//pnet->updctrl("ShiftInput/shift/mrs_natural/Decimation",512);

	MarSystem* fanout = mng.create("Fanout", "fanout");
	fanout->addMarSystem(makePitchNet(srate, 100.0, pitchSink));
	fanout->addMarSystem(makeAmplitudeNet(ampSink));
	pnet->addMarSystem(fanout);

	while ( pnet->getctrl("mrs_bool/notEmpty")->toBool() )
		pnet->tick();

	pitchList = getPitchesFromRealvecSink(pitchSink, srate);
	ampList = getAmpsFromRealvecSink(ampSink);
	boundaries = new realvec(2);
	(*boundaries)(0) = 0;
	(*boundaries)(1) = pitchList->getSize();
	delete pnet;
}

realvec*
Transcriber::getPitchesFromAudio(const string audioFilename)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	mrs_real srate = Easymar::addFileSource(pnet, audioFilename);
	MarSystem* rvSink = mng.create("RealvecSink", "rvSink");
	pnet->addMarSystem(makePitchNet(srate, 100.0, rvSink));

	while ( pnet->getctrl("mrs_bool/notEmpty")->toBool() )
		pnet->tick();

	realvec* pitchList = getPitchesFromRealvecSink(rvSink, srate);
	delete pnet;
	return pitchList;
}

realvec*
Transcriber::getPitchesFromRealvecSink(MarSystem* rvSink, const mrs_real srate)
{
	realvec data = rvSink->getctrl("mrs_realvec/data")->toVec();
	rvSink->updctrl("mrs_bool/done", true);

	realvec* pitchList = new realvec(data.getSize()/2);
	mrs_real pitchOutput;
	for (mrs_natural i=0; i<pitchList->getSize(); i++)
	{
		// on linux (but not OSX), we have pitchOutput of 0.5 if the pitch
		// detection can't decide on a pitch.
		pitchOutput = data(2*i+1);
		if (pitchOutput > 1)
			(*pitchList)(i) = samples2hertz( pitchOutput, srate);
		else
			(*pitchList)(i) = 0;
	}
	return pitchList;
}

realvec*
Transcriber::getAmpsFromRealvecSink(MarSystem* rvSink)
{
	realvec data = rvSink->getctrl("mrs_realvec/data")->toVec();
	rvSink->updctrl("mrs_bool/done", true);
	realvec* ampList = new realvec(data.getSize());
	(*ampList) = data;
	(*ampList) /= ampList->maxval();
	return ampList;
}

void
Transcriber::toMidi(realvec* pitchList)
{
	pitchList->apply( hertz2pitch );
}

void
Transcriber::appendRealvec(realvec* orig, const realvec* newValues)
{
	mrs_natural origSize = orig->getSize();
//   don't get cute unless everything else is working.  -gp
//	if (origSize == 0)
//	{
//		(*orig) = (*newValues);
//	}
//	else {
	mrs_natural newSize = origSize + newValues->getSize();
	//cout<<newSize<<endl;
	orig->stretch( newSize );
	for (mrs_natural i=0; i<newValues->getSize(); i++)
		(*orig)(origSize + i) = (*newValues)(i);
//	}
	delete newValues;
}

realvec*
Transcriber::getSubVector(const realvec* list, mrs_natural start,
                          mrs_natural length)
{
	realvec *subVector = new realvec(length);
	for (mrs_natural i=0; i<length; i++)
		(*subVector)(i) = (*list)(start + i);
	return subVector;
}

void
Transcriber::pitchSegment(realvec* pitchList, realvec* boundaries)
{
	realvec *region, *newBoundaries, *regionBounds;
	mrs_natural start, length;
	newBoundaries = new realvec;
	for (mrs_natural i=0; i<boundaries->getSize()-1; i++)
	{
		start = (mrs_natural) (*boundaries)(i);
		length = (mrs_natural) ((*boundaries)(i+1) - (*boundaries)(i));
		region = getSubVector(pitchList, start, length);
		regionBounds = findPitchBoundaries(region);
		(*regionBounds) += start;
		/*
				cout<<"i ="<<i<<endl;
				cout<<(*newBoundaries);
				cout<<(*regionBounds);
		*/
		appendRealvec(newBoundaries, regionBounds);
//		cout<<"done i ="<<i<<endl;
	}
//	cout<<"blah"<<endl;
	appendRealvec(boundaries, newBoundaries);
	boundaries->sort();
}

realvec
Transcriber::segmentRealvec(const realvec list, const realvec boundaries)
{
	mrs_natural maxCols=0;
	realvec newList;
	newList.create(boundaries.getSize()-1,10);

	if (boundaries.getSize() == 1)
		return list;

	mrs_natural note = 0;
	mrs_natural prevBound = (mrs_natural) boundaries(note);
	mrs_natural nextBound = (mrs_natural) boundaries(note+1);
	for (mrs_natural i=0; i<list.getSize(); i++)
	{
		if (i == nextBound )
		{
			if ((i-prevBound) > maxCols)
				maxCols = (i-prevBound);
			note++;
			prevBound = nextBound;
			nextBound = (mrs_natural) boundaries(note+1);
		}
		newList.stretchWrite(note, i - prevBound, list(i) );
	}
	newList.stretch(boundaries.getSize(), maxCols);
	return newList;
}

realvec*
Transcriber::findPitchBoundaries(const realvec* pitchList)
{
	mrs_natural minSpace = 8;
	mrs_real noteBoundary = 0.5;

	realvec* boundaries = new realvec(1);
	mrs_natural onsetIndex=0;

	mrs_real median;
	mrs_real prevNote=0.0;
	mrs_natural prevSamp=0;
	for (mrs_natural i=minSpace; i<pitchList->getSize()-minSpace; i++)
	{
		median = findMedian(i-minSpace/2, minSpace, pitchList);
		if ( fabs(median-prevNote) > noteBoundary )
		{
			if (i>prevSamp+minSpace)
			{
				prevNote = median;
				prevSamp = i;
				boundaries->stretchWrite( onsetIndex, i);
				onsetIndex++;
			}
			else
			{
				prevNote = median;
			}
		}
	}
	boundaries->stretch(onsetIndex);
	return boundaries;
}


void
Transcriber::ampSegment(realvec* ampList, realvec* boundaries)
{
	realvec *region, *newBoundaries, *regionBounds;
	mrs_natural start, length;
	newBoundaries = new realvec;
	for (mrs_natural i=0; i<boundaries->getSize()-1; i++)
	{
		start = (mrs_natural) (*boundaries)(i);
		length = (mrs_natural) ((*boundaries)(i+1) - (*boundaries)(i));
		region = getSubVector(ampList, start, length);
		regionBounds = findValleys(region);
		(*regionBounds) += start;
		appendRealvec(newBoundaries, regionBounds);
	}
	appendRealvec(boundaries, newBoundaries);
	boundaries->sort();
}

realvec*
Transcriber::findValleys(const realvec* list)
{
	realvec* valleys = new realvec(1);

	mrs_natural valIndex = 0;

	mrs_real localMin;
	mrs_real maxValue = 1.0;
	mrs_natural minSpace = 8;
	mrs_natural prevValIndex = 0;
	mrs_real prevValValue = 1.0;
	for (mrs_natural i=minSpace; i<list->getSize()-minSpace; i++)
	{
		if ( ((*list)(i) < (*list)(i-1)) &&
		        ((*list)(i) < (*list)(i+1)) &&
		        ((*list)(i) < maxValue) )
		{
			localMin = (*list)(i);
			if (i < prevValIndex+minSpace)
			{
				if (localMin < prevValValue)
				{
					// replace previous valley with this one
					(*valleys)(valIndex-1) = i;
					prevValIndex = i;
					prevValValue = localMin;
					//cout<<"***** fix bound "<<i<<endl;
				}
			}
			else
			{
				// new valley found
				valleys->stretchWrite(valIndex, i);
				valIndex++;
				prevValIndex = i;
				prevValValue = localMin;
				//cout<<"*** new bound "<<i<<endl;
			}
		}
	}
	valleys->stretch(valIndex);
	return valleys;
}


// find median without 0s.
mrs_real
Transcriber::findMedian(const mrs_natural start, const mrs_natural
                        length, const realvec* array)
{
	if ( length<=0 )
		return 0;
	realvec noZeros;
	noZeros.create(length);
	mrs_natural j=0;
	// don't include 0s
	for (mrs_natural i=0; i<length; i++)
	{
		if ( (*array)(start+i) > 0 )
		{
			noZeros(j)=(*array)(start+i);
			j++;
		}
	}
	noZeros.stretch(j-1);
	if (j-1 > 0)
		return noZeros.median();
	return 0;
}


/*
realvec
Transcriber::getRelativeDurations(realvec boundaries)
{
	boundaries.sort();
//	cout<<boundaries;

	realvec durations;
	durations.create( boundaries.getSize()-1 );

	mrs_natural i;
	mrs_natural min = 99999; // infinity
	// calculate durations in samples
	// and find smallest
	for (i=0; i<boundaries.getSize()-1; i++)
	{
		durations(i) = boundaries(i+1) - boundaries(i);
//		cout<<"duration: "<<durations(i)<<endl;
		// we don't care about silent durations
		if (durations(i) < min)
			min = (mrs_natural) durations(i);
	}
//	cout<<"min: "<<min<<endl;
	// find relative durations
	// yes, we want to truncate the division.
	for (i=0; i<boundaries.getSize()-1; i++)
	{
		//durations(i) = (mrs_natural) ( durations(i) / (min) );
		durations(i) = (mrs_natural) ( durations(i) / (min*0.9) );
	}
	//cout<<"**********"<<endl;
	return durations;
}
*/

realvec*
Transcriber::getNotes(const realvec* pitchList, const realvec* ampList,
                      const realvec*
                      boundaries)
{
	mrs_natural numNotes = boundaries->getSize();
	realvec* notes = new realvec(numNotes-1, 2);
//	pitchList = segmentRealvec(pitchList, boundaries);
//	ampList = segmentRealvec(ampList, boundaries);

	//realvec durs = getRelativeDurations(boundaries);
//	realvec notePitches, noteAmps;
	mrs_natural start, length;
	realvec* region;
	for (mrs_natural i=0; i<numNotes-1; i++)
	{
		start = (mrs_natural) (*boundaries)(i);
		length = (mrs_natural) ((*boundaries)(i+1) - (*boundaries)(i));
		region = getSubVector(pitchList, start, length);
		(*notes)(i,0) = findMedian(0, length, region);
		(*notes)(i,1) = (*boundaries)(i+1)-(*boundaries)(i);
	}
	return notes;
}


/*
void
Transcriber::setOptions(mrs_natural getRadius, mrs_real getNewNote, mrs_real getCertantyDiv)
{
	window = getRadius;
	noteBoundary = getNewNote;
	pitch_certamrs_naturaly_div = getCertantyDiv;
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
	//return (mrs_natural) round( second*44100.0/512.0 ); //round() does not exist in <cmath> [!]
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
	mrs_natural onsetIndex=1;
	mrs_natural prevSamp=0;
	for (i=window; i<pitchList.getSize()-window; i++) {
		median = findMedian(i-window, 2*window, pitchList);
		if ( fabs(median-prevNote) > noteBoundary ) {
			if (i>prevSamp+window) {
				prevNote = median;
				prevSamp = i;
				onsets.stretchWrite( onsetIndex, i);
				onsetIndex++;
			}
			else {
				prevNote = median;
				prevSamp = i;
				onsets(onsetIndex) = i;
			}
		}
	}
	onsets.stretchWrite(onsetIndex, pitchList.getSize() );
	onsets.stretch(onsetIndex+1);
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
			if (distance < noteBoundary) {
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

