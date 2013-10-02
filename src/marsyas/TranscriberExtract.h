#ifndef MARSYAS_TRANSCRIBERINFO_H
#define MARSYAS_TRANSCRIBERINFO_H

#include <marsyas/system/MarSystemManager.h>
//#include "Easymar.h"

namespace Marsyas
{
/**
	\class TranscriberExtract
	\ingroup Notmar
	\brief A collection of functions which simplify transcription (pitch
extraction, amplitude extraction, etc).
*/

class TranscriberExtract
{

public:
  TranscriberExtract();
  ~TranscriberExtract();
  mrs_real addFileSource(MarSystem* net,
                         const std::string infile);

  realvec getPitchesFromAudio(const std::string audioFilename);
  realvec getAmpsFromAudio(const std::string audioFilename);
  MarSystem* makePitchNet(const mrs_real srate,
                          const mrs_real lowFreq = 100.0,
                          MarSystem* rvSink = NULL);
  realvec getPitchesFromRealvecSink(MarSystem* rvSink,
                                    const mrs_real srate);
  MarSystem* makeAmplitudeNet(MarSystem* rvSink = NULL);
  realvec getAmpsFromRealvecSink(MarSystem* rvSink);

  void getAllFromAudio(const std::string audioFilename, realvec&
                       pitchList, realvec& ampList, realvec&
                       boundaries);
  void toMidi(realvec& pitchList);
  mrs_real getNormalizingGain(const std::string audioFilename);

private:
  MarSystemManager mng;
};
}
#endif

