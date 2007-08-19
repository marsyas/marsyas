// currently refactoring this file.  -gp

/**
	\class TranscriberInfo
	\ingroup Notmar
	\brief A collection of functions which simplify transcription (pitch
extraction, amplitude extraction, etc).

	Usage:
	- setPitchList() or getPitchesFromAudio()
*/


#include "TranscriberInfo.h"
static MarSystemManager mng;

#include <iostream>

TranscriberInfo::TranscriberInfo()
{
}

TranscriberInfo::~TranscriberInfo()
{
}

MarSystem*
TranscriberInfo::makePitchNet(const mrs_real srate, const mrs_real lowFreq, MarSystem* rvSink)
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

MarSystem* TranscriberInfo::makeAmplitudeNet(MarSystem* rvSink)
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
TranscriberInfo::getAllFromAudio(const string audioFilename, realvec* &
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
TranscriberInfo::getPitchesFromAudio(const string audioFilename)
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
TranscriberInfo::getPitchesFromRealvecSink(MarSystem* rvSink, const mrs_real srate)
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
TranscriberInfo::getAmpsFromRealvecSink(MarSystem* rvSink)
{
	realvec data = rvSink->getctrl("mrs_realvec/data")->toVec();
	rvSink->updctrl("mrs_bool/done", true);
	realvec* ampList = new realvec(data.getSize());
	(*ampList) = data;
	(*ampList) /= ampList->maxval();
	return ampList;
}

void
TranscriberInfo::toMidi(realvec* pitchList)
{
	pitchList->apply( hertz2pitch );
}


