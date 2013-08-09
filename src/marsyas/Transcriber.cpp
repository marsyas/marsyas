#include "Transcriber.h"
#include <iostream>

namespace Marsyas
{

//static const mrs_natural MIN_NOTE_FRAMES = 9; // note length
#define MIN_NOTE_FRAMES 9
// sixteenth notes at 120 BPM would be 10.75 frames apart
// if each frame is 512 samples, or 11.6 ms.



Transcriber::Transcriber()
{
}

Transcriber::~Transcriber()
{
}

// general functions
mrs_real
Transcriber::findMedianWithoutZeros(const mrs_natural start,
                                    const mrs_natural length,
                                    const realvec& array)
{
  if ( length<=0 )
    return 0;
  realvec noZeros;
  noZeros.create(length);
  mrs_natural j=0;
  // don't include 0s
  for (mrs_natural i=0; i<length; ++i)
  {
    if ( array(start+i) > 0 )
    {
      noZeros(j)=array(start+i);
      j++;
    }
  }
  noZeros.stretch(j-1);
  if (j-1 <= 0)
    return 0;
  return noZeros.median();
}

realvec
Transcriber::findPeaks(const realvec& list, const mrs_real cutoff)
{
  realvec peaks(1);
  mrs_natural valIndex = 0;

  mrs_real localMax;
  mrs_natural minSpace = MIN_NOTE_FRAMES;
  mrs_natural prevValIndex = 0;
  mrs_real prevValValue = 1.0;
  for (mrs_natural i=minSpace; i<list.getSize()-minSpace; ++i)
  {
    if ( (list(i) > list(i-1)) &&
         (list(i) > list(i+1)) &&
         (list(i) > cutoff) )
    {
      localMax = list(i);
      if ((mrs_natural)i < prevValIndex+minSpace)
      {
        if (localMax > prevValValue)
        {
          // replace previous valley with this one
          peaks(valIndex-1) = i;
          prevValIndex = i;
          prevValValue = localMax;
        }
      }
      else
      {
        // new valley found
        peaks.stretchWrite(valIndex, i);
        valIndex++;
        prevValIndex = i;
        prevValValue = localMax;
      }
    }
  }
  peaks.stretch(valIndex);
  return peaks;
}

realvec
Transcriber::findValleys(const realvec& list)
{
  realvec valleys(1);
  mrs_natural valIndex = 0;

  mrs_real localMin;
  mrs_natural minSpace = MIN_NOTE_FRAMES;
  mrs_natural prevValIndex = 0;
  mrs_real prevValValue = 1.0;
  for (mrs_natural i=minSpace; i<list.getSize()-minSpace; ++i)
  {
    if ( (list(i) < list(i-1)) &&
         (list(i) < list(i+1)))
    {
      localMin = list(i);
      if ((mrs_natural)i < prevValIndex+minSpace)
      {
        if (localMin < prevValValue)
        {
          // replace previous valley with this one
          valleys(valIndex-1) = i;
          prevValIndex = i;
          prevValValue = localMin;
        }
      }
      else
      {
        // new valley found
        valleys.stretchWrite(valIndex, i);
        valIndex++;
        prevValIndex = i;
        prevValValue = localMin;
      }
    }
  }
  valleys.stretch(valIndex);
  return valleys;
}

mrs_real
Transcriber::findNextPeakValue(const realvec& list, const mrs_natural
                               start)
{
  mrs_natural i = start;
  mrs_bool isPeak = false;
  mrs_real minValue = 0.1;
  do
  {
    ++i;
    if (i == list.getSize())
      return 0.0;
    if ( (list(i) > list(i-1)) &&
         (list(i) > list(i+1)) &&
         ( list(i) > minValue) )
    {
      isPeak = true;
    }
  }
  while ( isPeak == false );
  return list(i);
}



// pitch stuff

void
Transcriber::pitchSegment(const realvec& pitchList, realvec&
                          boundaries, const mrs_natural width)
{
  if (boundaries.getSize() == 0)
  {
    boundaries.create(2);
    boundaries(0) = 0.0;
    boundaries(1) = pitchList.getSize();
  }
  realvec region, *newBoundaries, regionBounds;
  mrs_natural start, length;
  newBoundaries = new realvec;
  for (mrs_natural i=0; i<boundaries.getSize()-1; ++i)
  {
    start = (mrs_natural) boundaries(i);
    length = (mrs_natural) (boundaries(i+1) - boundaries(i));
    region = pitchList.getSubVector(start, length);
    regionBounds = findPitchBoundaries(region, width);
    regionBounds += start;
    newBoundaries->appendRealvec(regionBounds);
  }
  boundaries.appendRealvec(*newBoundaries);
  boundaries.sort();
}

realvec
Transcriber::findPitchBoundaries(const realvec& pitchList, const
                                 mrs_natural width)
{
  //mrs_natural minSpace = MIN_NOTE_FRAMES;
  mrs_natural minSpace = width;
  mrs_real noteBoundary = 0.5;

  realvec boundaries(1);
  mrs_natural onsetIndex=0;

  mrs_real median;
  mrs_real prevNote=0.0;
  mrs_natural prevSamp=0;
  for (mrs_natural i=minSpace; i<pitchList.getSize()-minSpace; ++i)
  {
    median = findMedianWithoutZeros(i-minSpace, 2*minSpace, pitchList);
    if ( fabs(median-prevNote) > noteBoundary )
    {
      if ((mrs_natural)i > prevSamp+minSpace)
      {
        prevNote = median;
        prevSamp = i;
        boundaries.stretchWrite( onsetIndex, i);
        onsetIndex++;
      }
      else
      {
        prevNote = median;
      }
    }
  }
  boundaries.stretch(onsetIndex);
  return boundaries;
}



// amplitude stuff
void
Transcriber::ampSegment(const realvec& ampList, realvec&
                        boundaries, const mrs_real cutoff)
{
  if (boundaries.getSize() == 0)
  {
    boundaries.create(2);
    boundaries(0) = 0.0;
    boundaries(1) = ampList.getSize()-1;
  }
  realvec region, *newBoundaries, regionBounds;
  mrs_natural start, length;
  newBoundaries = new realvec;
  for (mrs_natural i=0; i<boundaries.getSize()-1; ++i)
  {
    start = (mrs_natural) boundaries(i);
    length = (mrs_natural) (boundaries(i+1) - boundaries(i));
    region = ampList.getSubVector(start, length);
    //regionBounds = findValleys(region);
    regionBounds = findPeaks(region, cutoff);
//		filterAmpBoundaries(region, regionBounds);
    regionBounds += start;
    newBoundaries->appendRealvec(regionBounds);
  }
  boundaries.appendRealvec(*newBoundaries);
  boundaries.sort();
}

void
Transcriber::filterAmpBoundaries(realvec& regionAmps, realvec &regionBounds)
{
  if (regionBounds.getSize() < 2)
    return;

  // create empty list of regionBounds to keep
  mrs_natural numSamples = regionBounds.getSize();
  realvec newBounds(numSamples);
  mrs_natural newIndex=0;

  mrs_real regionMinVal = 0.1;
  /*
  	// ignore quiet parts
  	if ( regionAmps.mean() < regionMinVal )
  	{
  		newBounds.stretch(0);
  		regionBounds = newBounds;
  		return;
  	}
  */

  // normalize amps in pitch region
  regionAmps /= regionAmps.maxval();

  mrs_natural start, length;
  mrs_real valleyMinVal = 0.20;
  mrs_real valley;
  realvec region;
  for (mrs_natural i=0; i<regionBounds.getSize(); ++i)
  {
    start = (mrs_natural) regionBounds(i);
    if (i < regionBounds.getSize()-1 )
      length = (mrs_natural) (regionBounds(i+1) - regionBounds(i));
    else
      length = regionAmps.getSize() - i;
    region = regionAmps.getSubVector(start, length);

    valley = regionAmps(start);
    if ( (valley < valleyMinVal) &&
//		if ( (valley < peakRatio*findNextPeakValue(&region, 0)) &&
         (region.mean() > regionMinVal) )
    {
//			cout<<"at frame "<<start<<" keep valley: "<<valley<<endl;
      newBounds(newIndex) = start;
      newIndex++;
    }
    else
    {
//			cout<<"Removed "<<start<<endl;
//			cout<<valley<<"\t"<<region.mean()<<endl;
//			cout<<region;
    }
  }
  newBounds.stretch(newIndex);
//	cout<<"in: "<<regionBounds.getSize();
//	cout<<"\t out: "<<newBounds.getSize()<<endl;
  regionBounds = newBounds;
}



// note stuff
void
Transcriber::getRelativeDurations(const realvec& boundaries, realvec
                                  &durations)
{
  mrs_natural numNotes = boundaries.getSize()-1;
  durations.create(numNotes);

  mrs_natural i;
  mrs_natural min = 99999; // infinity
  // calculate durations in samples
  // and find smallest
  for (i=0; i<numNotes; ++i)
  {
    durations(i) = boundaries(i+1) - boundaries(i);
//		cout<<"duration: "<<(*durations)(i)<<endl;
    // we don't care about silent durations
    if (durations(i) < min)
      min = (mrs_natural) durations(i);
  }
//	cout<<"min: "<<min<<endl;
  // find relative durations
  // yes, we want to truncate the division.
  for (i=0; i<numNotes; ++i)
  {
    durations(i) = (mrs_natural) ( durations(i) / (min) );
  }
}

void
Transcriber::discardBeginEndSilences(const realvec& pitchList, const realvec&
                                     ampList, realvec& boundaries)
{
  // could be useful for an improved function.
  (void) ampList;

  mrs_real notePitch;
  mrs_natural i,j;
  mrs_natural start,length;
  // Remove beginning silences.
  i=0;
  start = (mrs_natural) boundaries(i);
  length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
  notePitch = findMedianWithoutZeros(start, length, pitchList);
  while ( (notePitch == 0) && (i < boundaries.getSize()-1) )
  {
    for (j=i; j<boundaries.getSize()-1; j++)
      boundaries(j) = boundaries(j+1);
    boundaries.stretch(j);
    ++i;
    start = (mrs_natural) boundaries(i);
    length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
    notePitch = findMedianWithoutZeros(start, length, pitchList);
  }
  // Remove ending silences.
  i=boundaries.getSize()-2;
  start = (mrs_natural) boundaries(i);
  length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
  notePitch = findMedianWithoutZeros(start, length, pitchList);
  while ( (notePitch == 0) && (i < boundaries.getSize()-1) )
  {
    boundaries.stretch(i+1);
    i--;
    start = (mrs_natural) boundaries(i);
    length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
    notePitch = findMedianWithoutZeros(start, length, pitchList);
  }
}

void
Transcriber::discardEndingTotalSilenceAmpsOnly(realvec& ampList)
{
  mrs_natural i = ampList.getSize()-1;
  // Remove ending silences.
  while ( (i>0) && (ampList(i) == 0.0))
  {
    i--;
  }
  ampList.stretch(i);
}

void
Transcriber::discardBeginEndSilencesAmpsOnly(const realvec& ampList,
    realvec& boundaries)
{
  mrs_real sampleAmp;
  mrs_natural i,j;
  i=0;
  // Remove beginning silences.
  mrs_natural start = (mrs_natural) boundaries(i);
  //mrs_natural length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
  sampleAmp = ampList(start);
  while ( (sampleAmp < 0.1) && (i < boundaries.getSize()-1) )
  {
    for (j=i; j<boundaries.getSize()-1; j++)
      boundaries(j) = boundaries(j+1);
    boundaries.stretch(j);
    ++i;
    start = (mrs_natural) boundaries(i);
    //length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
    sampleAmp = ampList(start);
  }
  // Remove ending silences.
  i=boundaries.getSize()-2;
  start = (mrs_natural) boundaries(i);
  //length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
  sampleAmp = ampList(start);
  while ( (sampleAmp < 0.1) && (i < boundaries.getSize()-1) )
  {
    boundaries.stretch(i+1);
    i--;
    start = (mrs_natural) boundaries(i);
    //length = (mrs_natural) ( boundaries(i+1)-boundaries(i) );
    sampleAmp = ampList(start);
  }
}


// this will not include relative durations
realvec
Transcriber::getNotes(const realvec& pitchList, const realvec& ampList,
                      const realvec& boundaries)
{
  (void) ampList;
  mrs_natural numNotes = boundaries.getSize()-1;
  realvec notes(numNotes, 2);

  mrs_natural start, length;
  mrs_real notePitch;

  mrs_natural boundIndex = 0;
  notePitch = findMedianWithoutZeros(0, (mrs_natural) boundaries(1),
                                     pitchList);
  if (notePitch == 0)
    boundIndex++;
  mrs_natural firstFrame = (mrs_natural) boundaries(boundIndex);
  for (mrs_natural i=0; i<numNotes; ++i)
  {
    notes(i,1) = boundaries(boundIndex) - firstFrame;

    // get approximate pitch
    start = (mrs_natural) boundaries(boundIndex);
    length = (mrs_natural) (boundaries(boundIndex+1) -
                            boundaries(boundIndex));
    notePitch = findMedianWithoutZeros(start, length, pitchList);

    notes(i,0) = notePitch;
    boundIndex++;
  }
  notes.stretch(boundIndex-2,2);
  return notes;
}

} //namespace

