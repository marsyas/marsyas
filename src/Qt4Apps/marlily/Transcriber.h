#ifndef MARSYAS_TRANSCRIBER_H
#define MARSYAS_TRANSCRIBER_H

#include <marsyas/system/MarSystemManager.h>
#include "TranscriberExtract.h"
using namespace std;
namespace Marsyas
{
/**
	\class Transcriber
	\ingroup Notmar
	\brief A collection of functions which simplify transcription
(detecting onsets via pitch and amplitudes, calculating notes, etc).

*/

class Transcriber: public TranscriberExtract
{

public:
  Transcriber();
  ~Transcriber();

  /** \name General fuctions */
/// non-Transcriber-specific functions, but I don't think they're
/// useful enough to add to realvec.
  //@{
  static mrs_real findMedianWithoutZeros(const mrs_natural start,
                                         const mrs_natural length,
                                         const realvec& array);
  static realvec findValleys(const realvec& list);
  static realvec findPeaks(const realvec& list);
  static mrs_real findNextPeakValue(const realvec& list,
                                    const mrs_natural start);
  //@}


  /** \name Pitch stuff */
  //@{
  static void pitchSegment(const realvec& pitchList, realvec& boundaries);
  static realvec findPitchBoundaries(const realvec& pitchList);
  //@}


  /** \name Amplitude stuff */
  //@{
  static void ampSegment(const realvec& ampList, realvec& boundaries);
  static void filterAmpBoundaries(realvec& ampList, realvec&
                                  boundaries);
  //@}


  /** \name Note stuff */
  //@{
  static realvec getNotes(const realvec& pitchList, const realvec&
                          ampList, const realvec& boundaries);
  static void discardBeginEndSilences(const realvec& pitchList, const
                                      realvec& ampList, realvec& boundaries);
  static void discardBeginEndSilencesAmpsOnly(const realvec& ampList,
      realvec& boundaries);
  static void getRelativeDurations(const realvec& boundaries,
                                   realvec &durations);
  //@}

};
}
#endif

