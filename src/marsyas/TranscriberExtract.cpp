#include "TranscriberExtract.h"
namespace Marsyas
{

//static MarSystemManager mng; // moved to private member

TranscriberExtract::TranscriberExtract()
{
}

TranscriberExtract::~TranscriberExtract()
{
}

mrs_real TranscriberExtract::addFileSource(MarSystem* net, const std::string infile)
{
  if (infile == EMPTYSTRING)
  {
    MRSERR("Please specify a sound file.");
    return 0;
  }
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updControl("SoundFileSource/src/mrs_string/filename", infile);
  net->linkControl("mrs_bool/hasData",
                   "SoundFileSource/src/mrs_bool/hasData");
  return net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
}


MarSystem*
TranscriberExtract::makePitchNet(const mrs_real srate, const mrs_real lowFreq, MarSystem* rvSink)
{
  mrs_real highFreq = 5000.0;

  MarSystem *net = mng.create("Series", "pitchNet");
  net->addMarSystem(mng.create("ShiftInput", "sfi"));
  net->addMarSystem(mng.create("PitchPraat", "pitch"));
  if (rvSink != NULL)
    net->addMarSystem(rvSink);

  // yes, this is the right way around (lowSamples<-highFreq)
  net->updControl("PitchPraat/pitch/mrs_natural/lowSamples",
                  hertz2samples(highFreq, srate) );
  net->updControl("PitchPraat/pitch/mrs_natural/highSamples",
                  hertz2samples(lowFreq, srate) );

  // The window should be just long enough to contain three periods
  // (for pitch detection) of MinimumPitch.
  mrs_real windowSize = 3.0/lowFreq*srate;
  net->updControl("mrs_natural/inSamples", 512);
  net->updControl("ShiftInput/sfi/mrs_natural/winSize",
                  powerOfTwo(windowSize));

  return net;
}

MarSystem* TranscriberExtract::makeAmplitudeNet(MarSystem* rvSink)
{
  MarSystem *net = mng.create("Series", "amplitudeNet");
  net->addMarSystem(mng.create("ShiftInput", "sfiAmp"));
  net->addMarSystem(mng.create("Rms", "rms"));
  if (rvSink != NULL)
    net->addMarSystem(rvSink);

  net->updControl("mrs_natural/inSamples", 512);
  net->updControl("ShiftInput/sfiAmp/mrs_natural/winSize", 512);

  return net;
}

void
TranscriberExtract::getAllFromAudio(const std::string audioFilename, realvec&
                                    pitchList, realvec& ampList,
                                    realvec& boundaries)
{
  MarSystem* pitchSink = mng.create("RealvecSink", "pitchSink");
  MarSystem* ampSink = mng.create("RealvecSink", "ampSink");

  MarSystem* pnet = mng.create("Series", "pnet");
  mrs_real srate = addFileSource(pnet, audioFilename);
// TODO: double the number of observations?
//	pnet->updControl("SoundFileSource/src/mrs_natural/inSamples",256);
//	pnet->addMarSystem(mng.create("ShiftInput", "shift"));
//	pnet->updControl("ShiftInput/shift/mrs_natural/winSize",512);

  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(makePitchNet(srate, 100.0, pitchSink));
  fanout->addMarSystem(makeAmplitudeNet(ampSink));
  pnet->addMarSystem(fanout);

  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
    pnet->tick();

  pitchList = getPitchesFromRealvecSink(pitchSink, srate);
  ampList = getAmpsFromRealvecSink(ampSink);
  boundaries.create(2);
  boundaries(0) = 0;
  boundaries(1) = pitchList.getSize();
  delete pnet;
}

realvec
TranscriberExtract::getPitchesFromAudio(const std::string audioFilename)
{
  mrs_real normalize = getNormalizingGain(audioFilename);

  MarSystem* pnet = mng.create("Series", "pnet");
  mrs_real srate = addFileSource(pnet, audioFilename);
  pnet->addMarSystem(mng.create("Gain", "normalizing"));
  pnet->updControl("Gain/normalizing/mrs_real/gain",normalize);
  MarSystem* rvSink = mng.create("RealvecSink", "rvSink");
  pnet->addMarSystem(makePitchNet(srate, 100.0, rvSink));

  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
    pnet->tick();

  realvec pitchList = getPitchesFromRealvecSink(rvSink, srate);
  delete pnet;
  return pitchList;
}

realvec
TranscriberExtract::getAmpsFromAudio(const std::string audioFilename)
{
  mrs_real normalize = getNormalizingGain(audioFilename);

  MarSystem* pnet = mng.create("Series", "pnet");
  addFileSource(pnet, audioFilename);

  pnet->addMarSystem(mng.create("Gain", "normalizing"));
  pnet->updControl("Gain/normalizing/mrs_real/gain",normalize);
  MarSystem* rvSink = mng.create("RealvecSink", "rvSink");
  pnet->addMarSystem(makeAmplitudeNet(rvSink));

  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
    pnet->tick();

  realvec rmsList = getAmpsFromRealvecSink(rvSink);
  delete pnet;

  // normalize RMS
  rmsList -= rmsList.minval();
  mrs_real maxRms = rmsList.maxval();
  if (maxRms != 0)
    rmsList /= maxRms;
  return rmsList;
}

realvec
TranscriberExtract::getPitchesFromRealvecSink(MarSystem* rvSink,
    const mrs_real srate)
{
  realvec data = rvSink->getctrl("mrs_realvec/data")->to<mrs_realvec>();
  rvSink->updControl("mrs_bool/done", true);

  realvec pitchList(data.getSize()/2);
  mrs_real pitchOutput;
  for (mrs_natural i=0; i<pitchList.getSize(); ++i)
  {
    // on linux (but not OSX), we have pitchOutput of 0.5 if the pitch
    // detection can't decide on a pitch.
    pitchOutput = data(2*i+1);
    if (pitchOutput > 1)
      pitchList(i) = samples2hertz( pitchOutput, srate);
    else
      pitchList(i) = 0;
  }
  return pitchList;
}

realvec
TranscriberExtract::getAmpsFromRealvecSink(MarSystem* rvSink)
{
  realvec data = rvSink->getctrl("mrs_realvec/data")->to<mrs_realvec>();
  rvSink->updControl("mrs_bool/done", true);
  realvec ampList(data.getSize());
  ampList = data;
  return ampList;
}

void
TranscriberExtract::toMidi(realvec& pitchList)
{
  pitchList.apply( hertz2pitch );
}

mrs_real
TranscriberExtract::getNormalizingGain(const std::string audioFilename)
{
  mrs_real maxVal = 0.0;

  MarSystem* pnet = mng.create("Series", "pnet");
  addFileSource(pnet, audioFilename);
  // forces Marsyas to write to processedData
  pnet->addMarSystem(mng.create("Gain", "null"));

  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
  {
    pnet->tick();
    const realvec& processedData =
      pnet->getctrl("SoundFileSource/src/mrs_realvec/processedData")->to<mrs_realvec>();
    for (mrs_natural i=0; i< processedData.getSize(); ++i)
    {
      mrs_real val = fabs(processedData(i));
      if (val > maxVal)
        maxVal = val;
    }
  }

  delete pnet;
  return 1.0/maxVal;
}

} // namespace Marsyas
