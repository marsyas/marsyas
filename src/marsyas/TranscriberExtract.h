#ifndef MARSYAS_TRANSCRIBERINFO_H
#define MARSYAS_TRANSCRIBERINFO_H

#include "MarSystemManager.h"
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

	static mrs_real addFileSource(MarSystem* net, const std::string
infile);

	static realvec* getPitchesFromAudio(const std::string audioFilename);
	static MarSystem* makePitchNet(const mrs_real srate, const mrs_real
	                               lowFreq = 100.0, MarSystem* rvSink = NULL);
	static realvec* getPitchesFromRealvecSink(MarSystem* rvSink, const mrs_real
	        srate);
	static MarSystem* makeAmplitudeNet(MarSystem* rvSink = NULL);
	static realvec* getAmpsFromRealvecSink(MarSystem* rvSink);

	static void getAllFromAudio(const std::string audioFilename, realvec* &
	                            pitchList, realvec* &ampList);
	static void toMidi(realvec* pitchList);

private:

};
}
#endif

