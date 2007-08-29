#include "Transcriber.h"
#include <iostream>
using namespace Marsyas;
using namespace std;

static MarSystemManager mng;

Transcriber::Transcriber()
{
}

Transcriber::~Transcriber()
{
}

// general functions
realvec*
Transcriber::segmentRealvec(const realvec* list, const realvec* boundaries)
{
	mrs_natural maxCols=0;
	realvec* newList = new realvec(boundaries->getSize()-1,10);

	if (boundaries->getSize() == 1)
		return NULL;

	mrs_natural note = 0;
	mrs_natural prevBound = (mrs_natural) (*boundaries)(note);
	mrs_natural nextBound = (mrs_natural) (*boundaries)(note+1);
	for (mrs_natural i=0; i<list->getSize(); i++)
	{
		if (i == nextBound )
		{
			if ((i-prevBound) > maxCols)
				maxCols = (i-prevBound);
			note++;
			prevBound = nextBound;
			nextBound = (mrs_natural) (*boundaries)(note+1);
		}
		newList->stretchWrite(note, i - prevBound, (*list)(i) );
	}
	newList->stretch(boundaries->getSize(), maxCols);
	return newList;
}

mrs_real
Transcriber::findMedianWithoutZeros(const mrs_natural start,
                                    const mrs_natural length,
                                    const realvec* array)
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
	if (j-1 <= 0)
		return 0;
	return noZeros.median();
}

realvec*
Transcriber::findValleys(const realvec* list)
{
	realvec* valleys = new realvec(1);
	mrs_natural valIndex = 0;

	mrs_real localMin;
	mrs_natural minSpace = 8;
	mrs_natural prevValIndex = 0;
	mrs_real prevValValue = 1.0;
	for (mrs_natural i=minSpace; i<list->getSize()-minSpace; i++)
	{
		if ( ((*list)(i) < (*list)(i-1)) &&
		        ((*list)(i) < (*list)(i+1)))
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

mrs_real
Transcriber::findNextPeakValue(const realvec* list, const mrs_natural
                               start)
{
	mrs_natural i = start;
	mrs_bool isPeak = false;
	mrs_real minValue = 0.1;
	do
	{
		i++;
		if (i == list->getSize())
			return 0.0;
		if ( ((*list)(i) > (*list)(i-1)) &&
		        ((*list)(i) > (*list)(i+1)) &&
		        ( (*list)(i) > minValue) )
		{
			isPeak = true;
		}
	}
	while ( isPeak == false );
	return (*list)(i);
}



// pitch stuff

void
Transcriber::pitchSegment(realvec* pitchList, realvec* boundaries)
{
	realvec region, *newBoundaries, *regionBounds;
	mrs_natural start, length;
	newBoundaries = new realvec;
	for (mrs_natural i=0; i<boundaries->getSize()-1; i++)
	{
		start = (mrs_natural) (*boundaries)(i);
		length = (mrs_natural) ((*boundaries)(i+1) - (*boundaries)(i));
		region = pitchList->getSubVector(start, length);
		regionBounds = findPitchBoundaries(&region);
		(*regionBounds) += start;
		newBoundaries->appendRealvec(*regionBounds);
	}
	boundaries->appendRealvec(*newBoundaries);
	boundaries->sort();
}

realvec*
Transcriber::findPitchBoundaries(const realvec* pitchList)
{
	mrs_natural minSpace = 10;
	mrs_real noteBoundary = 0.5;

	realvec* boundaries = new realvec(1);
	mrs_natural onsetIndex=0;

	mrs_real median;
	mrs_real prevNote=0.0;
	mrs_natural prevSamp=0;
	for (mrs_natural i=minSpace; i<pitchList->getSize()-minSpace; i++)
	{
		median = findMedianWithoutZeros(i-minSpace, 2*minSpace, pitchList);
//		cout<<i<<"\t"<<(*pitchList)(i)<<"\t"<<median<<endl;
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


// amplitude stuff
void
Transcriber::ampSegment(realvec* ampList, realvec* boundaries)
{
	realvec region, *newBoundaries, *regionBounds;
	mrs_natural start, length;
	newBoundaries = new realvec;
	for (mrs_natural i=0; i<boundaries->getSize()-1; i++)
	{
		start = (mrs_natural) (*boundaries)(i);
		length = (mrs_natural) ((*boundaries)(i+1) - (*boundaries)(i));
		region = ampList->getSubVector(start, length);
		regionBounds = findValleys((&region));
		findAmpBoundaries(&region, regionBounds);
		(*regionBounds) += start;
		newBoundaries->appendRealvec(*regionBounds);
	}
	boundaries->appendRealvec(*newBoundaries);
	boundaries->sort();
}

// filter list of valleys
void
Transcriber::findAmpBoundaries(realvec* ampList, realvec* &boundaries)
{
	if (boundaries->getSize() <2)
		return;
	mrs_natural numSamples = boundaries->getSize();
	realvec *newBounds = new realvec(numSamples);
//	(*newBounds)(0) = 0;
	mrs_natural newIndex=0;

//	mrs_natural window = 10;
	mrs_real peakRatio = 0.8;
	realvec region;
	mrs_natural start, length;
	mrs_real valley;
	for (mrs_natural i=0; i<boundaries->getSize()-1; i++)
	{
		start = (mrs_natural) (*boundaries)(i);
		length = (mrs_natural) ((*boundaries)(i+1) - (*boundaries)(i));
		region = ampList->getSubVector(start, length);
		valley = (*ampList)(start);
		//cout<<start<<"\t"<<findNextPeakValue(region, 0)<<endl;
		if ( (valley < peakRatio*findNextPeakValue(&region, 0)) &&
		        (region.mean() > 0.01) )
		{
			(*newBounds)(newIndex) = start;
			newIndex++;
		}
		else
		{
			//cout<<"Removed "<<start<<endl;
			//cout<<valley<<"\t"<<findNextPeakValue(region, 0)<<endl;
		}
	}
	(*newBounds)(newIndex) = (*boundaries)( numSamples-1 );
	newBounds->stretch(newIndex+1);
	delete boundaries;
	boundaries = newBounds;
}


// note stuff
void
Transcriber::getRelativeDurations(const realvec* boundaries, realvec*
                                  &durations)
{
	durations = new realvec( boundaries->getSize()-1 );

	mrs_natural i;
	mrs_natural min = 99999; // infinity
	// calculate durations in samples
	// and find smallest
	for (i=0; i<boundaries->getSize()-1; i++)
	{
		(*durations)(i) = (*boundaries)(i+1) - (*boundaries)(i);
//		cout<<"duration: "<<(*durations)(i)<<endl;
		// we don't care about silent durations
		if ((*durations)(i) < min)
			min = (mrs_natural) (*durations)(i);
	}
//	cout<<"min: "<<min<<endl;
	// find relative durations
	// yes, we want to truncate the division.
	for (i=0; i<boundaries->getSize()-1; i++)
	{
		(*durations)(i) = (mrs_natural) ( (*durations)(i) / (min) );
	}
//	cout<<(*durations);
}

realvec*
Transcriber::getNotes(const realvec* pitchList, const realvec* ampList,
                      const realvec* boundaries)
{
	// FIXME Unused parameter
	(void) ampList;
	mrs_natural numNotes = boundaries->getSize();
	realvec* notes = new realvec(numNotes-1, 2);
//	realvec* notes = new realvec(1, 2);

	mrs_natural start, length;
//	realvec* region;
//	mrs_natural same=1;
//	mrs_natural prevSample=0;
//	mrs_real prevPitch=0.0;
	realvec* durations = new realvec();
	getRelativeDurations(boundaries, durations);
	//cout<<(*durations);
	for (mrs_natural i=0; i<numNotes-1; i++)
	{
		(*notes)(i,1) = (*durations)(i);

		start = (mrs_natural) (*boundaries)(i);
		length = (mrs_natural) ((*boundaries)(i+1) - (*boundaries)(i));
		(*notes)(i,0) = findMedianWithoutZeros(start, length, pitchList);
		//region = getSubVector(pitchList, start, length);
		/*
				if (region->getSize() > 0)
				{
					mrs_real regionPitch = round( region->median() );
					if (regionPitch == prevPitch)
						same++;
					else
					{
						prevPitch = regionPitch;
						same=1;
						prevSample = start;
						//cout<<"-----"<<endl;
					}
		*/
		//	cout<<(*boundaries)(i)<<"\t"<<length<<"\t"<<regionPitch<<"\t"<<same<<"\t"<<start-prevSample<<endl;
		//cout<<(*boundaries)(i)<<"\t"<<length<<"\t"<<regionPitch<<"\t"<<regionSTD<<"\t"<<same<<"\t"<<start-prevSample<<endl;
		//cout<<(*boundaries)(i)<<"\t"<<regionSTD<<endl;
		//(*notes)(i,0) = findMedianWithoutZeros(0, length, region);
		//(*notes)(i,1) = (*boundaries)(i+1)-(*boundaries)(i);
	}
//cout<<(*notes);
	return notes;
}



