/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/



#include <cstdio>

#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/mididevices.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>


using namespace std;
using namespace Marsyas;

string fileName;
string pluginName;
string methodopt;
CommandLineOptions cmd_options;


int helpopt;
int usageopt;
long offset = 0;
long duration = 1000 * 44100;
long band;
mrs_natural bandopt = 0;

float start = 0.0f;
float length = 1000.0f;
float gain = 1.0f;
float repetitions = 1;



void
printUsage(string progName)
{
  MRSDIAG("waveletplay.cpp - printUsage");
  cerr << "Usage : " << progName << " [-m method] [-g gain] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-f outputfile] [-p pluginName] [-r repetitions] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("boomchick.cpp - printHelp");
  cerr << "boomchick, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-f --file       : output to file " << endl;
  cerr << "-v --verbose    : verbose output" << endl;
  cerr << "-m --method     : tempo induction method " << endl;
  cerr << "-g --gain       : linear volume gain " << endl;
  cerr << "-o --offset     : playback start offset in samples " << endl;
  cerr << "-d --duration   : playback duration in samples     " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << "-p --plugin     : output plugin name " << endl;
  cerr << "-r --repetitions: number of repetitions " << endl;



  exit(1);
}




void
tempo_bcWavelet(string sfName, string resName)
{
  MarSystemManager mng;
  mrs_natural nChannels;
  mrs_real srate = 0.0;

  // prepare network
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  // wavelet filterbank
  total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  total->addMarSystem(mng.create("WaveletBands", "wvbnds"));
  // for each channel of filterbank extract envelope
  total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  total->addMarSystem(mng.create("OnePole", "lpf"));
  total->addMarSystem(mng.create("Norm", "norm"));
  {
    // Extra gain added for compensating the cleanup of the Norm Marsystem,
    // which used a 0.05 internal gain for some unknown reason.
    // \todo is this weird gain factor actually required?
    total->addMarSystem(mng.create("Gain", "normGain"));
    total->updctrl("Gain/normGain/mrs_real/gain", 0.05);
  }

  total->addMarSystem(mng.create("FullWaveRectifier", "fwr1"));
  total->addMarSystem(mng.create("ClipAudioRange", "clp"));


  cout << "NETWORK PREPARED" << endl;


  // prepare filename for reading
  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
  mrs_natural hopSize = winSize;
  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
  srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);
  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);

  // wavelt filterbank envelope extraction controls
  total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);

  // prepare vectors for processing
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
               total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec lowwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec hiwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec plowwin(1,
                  total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  realvec phiwin(1,
                 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  realvec bands(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());


  mrs_natural samplesPlayed = 0;


  // MarSystem* dest = mng.create("AudioSink", "dest");
  MarSystem* lowdest = mng.create("SoundFileSink", "lowdest");
  MarSystem* hidest  = mng.create("SoundFileSink", "hidest");
  MarSystem* plowdest = mng.create("SoundFileSink", "plowdest");
  MarSystem* phidest = mng.create("SoundFileSink", "phidest");


  mrs_natural onSamples = total->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural inSamples = total->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  // mrs_natural onObs = total->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  // mrs_natural inObs = total->getctrl("mrs_natural/inObservations")->to<mrs_natural>();


  // Peak pickers for high and low band
  MarSystem* lowpkr = mng.create("PeakerAdaptive", "lowpkr");
  lowpkr->updctrl("mrs_natural/inSamples", inSamples);
  lowpkr->updctrl("mrs_real/peakSpacing", 0.1);
  lowpkr->updctrl("mrs_real/peakStrength", 0.5);
  lowpkr->updctrl("mrs_natural/peakStart", 0);
  lowpkr->updctrl("mrs_natural/peakEnd", inSamples);
  lowpkr->updctrl("mrs_real/peakGain", 1.0);


  MarSystem* hipkr = mng.create("PeakerAdaptive", "hipkr");
  hipkr->updctrl("mrs_natural/inSamples", inSamples);
  hipkr->updctrl("mrs_real/peakSpacing", 0.05);
  hipkr->updctrl("mrs_real/peakStrength", 0.6);
  hipkr->updctrl("mrs_natural/peakStart", 0);
  hipkr->updctrl("mrs_natural/peakEnd", inSamples);
  hipkr->updctrl("mrs_real/peakGain", 1.0);




  lowdest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  hidest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  plowdest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  phidest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));

  lowdest->updctrl("mrs_real/israte", srate);
  lowdest->updctrl("mrs_string/filename", "lowband.wav");

  hidest->updctrl("mrs_real/israte", srate);
  hidest->updctrl("mrs_string/filename", "hiband.wav");

  plowdest->updctrl("mrs_real/israte", srate);
  plowdest->updctrl("mrs_string/filename", "plowband.wav");

  phidest->updctrl("mrs_real/israte", srate);
  phidest->updctrl("mrs_string/filename", "phiband.wav");

  cout << "BOOM-CHICK PROCESSING" << endl;
  cout << "sfName = " << sfName << endl;

  vector<mrs_natural> lowtimes;
  vector<mrs_natural> hitimes;

  while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    total->process(iwin, bands);
    for (mrs_natural t=0; t < onSamples; t++)
      lowwin(0,t) = bands(1, t);

    for (mrs_natural t=0; t < onSamples; t++)
      hiwin(0,t) = bands(3, t);


    lowpkr->process(lowwin, plowwin);
    hipkr->process(hiwin, phiwin);

    lowdest->process(lowwin, lowwin);
    hidest->process(hiwin, hiwin);

    plowdest->process(plowwin, plowwin);
    phidest->process(phiwin, phiwin);


    for (mrs_natural t=0; t < onSamples; t++)
      if (plowwin(0,t) > 0.0)
        lowtimes.push_back(samplesPlayed+t);

    for (mrs_natural t=0; t < onSamples; t++)
      if (phiwin(0,t) > 0.0)
        hitimes.push_back(samplesPlayed+t);

    samplesPlayed += onSamples;
  }

  cout << "Done with first loop" << endl;




  vector<mrs_natural>::iterator vi;

  MarSystem* playback = mng.create("Series", "playback");
  MarSystem* mix = mng.create("Fanout", "mix");
  mix->addMarSystem(mng.create("SoundFileSource", "orsrc"));
  mix->updctrl("SoundFileSource/orsrc/mrs_bool/mute", true);
  mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
  mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

  playback->addMarSystem(mix);
  playback->addMarSystem(mng.create("Sum", "sum"));
  playback->addMarSystem(mng.create("SoundFileSink", "adest"));
  cout << "SOUNDFILESINK srate = " << srate << endl;




  playback->updctrl("Fanout/mix/SoundFileSource/orsrc/mrs_string/filename", sfName);


  string sdname;
  string bdname;

  if (srate == 22050.0)
  {
    sdname = "./sd22k.wav";
    bdname = ".,/bd22k.wav";
  }
  else 				// assume everything is either 22k or 44.1k
  {
    sdname = "./sd.wav";
    bdname = "./bd.wav";
  }


  samplesPlayed = 0;
  onSamples = playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural lowtindex = 0;
  mrs_natural hitindex = 0;


  playback->updctrl("mrs_real/israte", srate);
  playback->updctrl("SoundFileSink/adest/mrs_string/filename", "boomchick.wav");


  cout << "******PLAYBACK******" << endl;

  while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/hasData")->to<mrs_bool>())
  {
    if (lowtimes[lowtindex] < samplesPlayed)
    {
      lowtindex++;

      if (lowtindex > 1)
        cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
      playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
      playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);
    }

    if (hitimes[hitindex] < samplesPlayed)
    {
      hitindex++;
      playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_string/filename", sdname);
      playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_natural/pos", 0);
    }
    playback->tick();
    samplesPlayed += onSamples;
  }



  cout << "FINISHED PROCESSING " << endl;
  cout << "audacity " << sfName << " band.wav peak.wav" << endl;
  delete hidest;
  delete lowdest;
  delete total;
}


void
tempo_bcFilter(string sfName, string resName)
{

  cout << "BOOMCICK_Filter PROCESSING" << endl;

  MarSystemManager mng;
  mrs_natural nChannels;
  mrs_real srate = 0.0;

  // prepare network
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  total->addMarSystem(mng.create("Sum", "sum"));
  total->addMarSystem(mng.create("Gain", "tgain"));
  // total->addMarSystem(mng.create("AudioSink", "dest"));

  total->addMarSystem(mng.create("DeviBot", "devibot"));

  /* Esitar* esitar = new Esitar("esitar");
     total->addMarSystem(esitar);
  */


  // high and low bandpass filters
  MarSystem *filters = mng.create("Fanout", "filters");
  realvec al(5),bl(5);

  al(0) = 1.0;
  al(1) = -3.9680;
  al(2) = 5.9062;
  al(3) = -3.9084;
  al(4) = 0.9702;

  bl(0) = 0.0001125;
  bl(1) = 0.0;
  bl(2) = -0.0002250;
  bl(3) = 0.0;
  bl(4) = 0.0001125;

  MarSystem *lfilter = mng.create("Series", "lfilter");
  lfilter->addMarSystem(mng.create("Filter", "llfilter"));
  lfilter->updctrl("Filter/llfilter/mrs_realvec/ncoeffs", bl);
  lfilter->updctrl("Filter/llfilter/mrs_realvec/dcoeffs", al);
  filters->addMarSystem(lfilter);

  realvec ah(5),bh(5);
  ah(0) = 1.0;
  ah(1) = -3.5797;
  ah(2) = 4.9370;
  ah(3) = -3.1066;
  ah(4) = 0.7542;

  bh(0) = 0.0087;
  bh(1) = 0.0;
  bh(2) = -0.0174;
  bh(3) = 0;
  bh(4) = 0.0087;

  MarSystem *hfilter = mng.create("Series", "hfilter");
  hfilter->addMarSystem(mng.create("Filter", "hhfilter"));
  hfilter->addMarSystem(mng.create("Gain", "gain"));
  hfilter->updctrl("Filter/hhfilter/mrs_realvec/ncoeffs", bh);
  hfilter->updctrl("Filter/hhfilter/mrs_realvec/dcoeffs", ah);
  filters->addMarSystem(hfilter);

  total->addMarSystem(filters);

  // prepare filename for reading
  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  // total->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  mrs_natural ch = total->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();
  mrs_real tg = 1.0 / ch;
  total->updctrl("Gain/tgain/mrs_real/gain", tg);
  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 2048;

  mrs_natural hopSize = winSize;
  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
  srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);
  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);

  // prepare vectors for processing
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
               total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec lowwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec hiwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec plowwin(1,
                  total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  realvec phiwin(1,
                 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  realvec bands(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());


  mrs_natural samplesPlayed = 0;

  // MarSystem* dest = mng.create("AudioSink", "dest");
  MarSystem* lowdest = mng.create("SoundFileSink", "lowdest");
  MarSystem* hidest  = mng.create("SoundFileSink", "hidest");
  MarSystem* plowdest = mng.create("SoundFileSink", "plowdest");
  MarSystem* phidest = mng.create("SoundFileSink", "phidest");


  mrs_natural onSamples = total->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural inSamples = total->getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  // Peak pickers for high and low band
  MarSystem* lowpkr = mng.create("PeakerAdaptive", "lowpkr");
  lowpkr->updctrl("mrs_natural/inSamples", inSamples);
  lowpkr->updctrl("mrs_real/peakSpacing", 0.3);
  lowpkr->updctrl("mrs_real/peakStrength", 0.7);
  lowpkr->updctrl("mrs_natural/peakStart", 0);
  lowpkr->updctrl("mrs_natural/peakEnd", inSamples);
  lowpkr->updctrl("mrs_real/peakGain", 1.0);
  lowpkr->updctrl("mrs_natural/peakStrengthReset", 4);
  lowpkr->updctrl("mrs_real/peakDecay", 0.9);

  MarSystem* hipkr = mng.create("PeakerAdaptive", "hipkr");
  hipkr->updctrl("mrs_natural/inSamples", inSamples);
  hipkr->updctrl("mrs_real/peakSpacing", 0.3);
  hipkr->updctrl("mrs_real/peakStrength", 0.7);
  hipkr->updctrl("mrs_natural/peakStart", 0);
  hipkr->updctrl("mrs_natural/peakEnd", inSamples);
  hipkr->updctrl("mrs_real/peakGain", 1.0);
  hipkr->updctrl("mrs_natural/peakStrengthReset", 4);
  hipkr->updctrl("mrs_real/peakDecay", 0.9);

  lowdest->updctrl("mrs_natural/inSamples",
                   total->getctrl("mrs_natural/onSamples"));
  hidest->updctrl("mrs_natural/inSamples",
                  total->getctrl("mrs_natural/onSamples"));
  plowdest->updctrl("mrs_natural/inSamples",
                    total->getctrl("mrs_natural/onSamples"));
  phidest->updctrl("mrs_natural/inSamples",
                   total->getctrl("mrs_natural/onSamples"));

  lowdest->updctrl("mrs_real/israte", srate);
  lowdest->updctrl("mrs_string/filename", "lowband.wav");

  hidest->updctrl("mrs_real/israte", srate);
  hidest->updctrl("mrs_string/filename", "hiband.wav");

  plowdest->updctrl("mrs_real/israte", srate);
  plowdest->updctrl("mrs_string/filename", "plowband.wav");

  phidest->updctrl("mrs_real/israte", srate);
  phidest->updctrl("mrs_string/filename", "phiband.wav");

  cout << "BOOM-CHICK PROCESSING" << endl;
  vector<mrs_natural> lowtimes;
  vector<mrs_natural> hitimes;

  // Initialize vectors for file writing
  //int r;
  //int len;
  //len = 5500;
  //realvec thumb(len);




  while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    total->process(iwin, bands);


    for (mrs_natural t=0; t < onSamples; t++)
      lowwin(0,t) = bands(0, t);



    for (mrs_natural t=0; t < onSamples; t++)
      hiwin(0,t) = bands(1, t);


    lowpkr->process(lowwin, plowwin);
    hipkr->process(hiwin, phiwin);

    lowdest->process(lowwin, lowwin);
    hidest->process(hiwin, hiwin);

    plowdest->process(plowwin, plowwin);
    phidest->process(phiwin, phiwin);


    for (mrs_natural t=0; t < onSamples; t++)
    {
      if (plowwin(0,t) > 0.0)
      {
        lowtimes.push_back(samplesPlayed+t);
#ifdef MARSYAS_MIDIIO
        total->updctrl("DeviBot/devibot/mrs_natural/arm", DEVIBOT_NA);

        total->updctrl("DeviBot/devibot/mrs_velocity/byte3", 50);

        total->updctrl("DeviBot/devibot/mrs_bool/strike", true);
#endif


      }

      for (mrs_natural t=0; t < onSamples; t++)
      {
        if (phiwin(0,t) > 0.0)
        {
          hitimes.push_back(samplesPlayed+t);

#ifdef MARSYAS_MIDIIO
          total->updctrl("DeviBot/devibot/mrs_natural/arm", DEVIBOT_GE);

          total->updctrl("DeviBot/devibot/mrs_velocity/byte3", 50);

          total->updctrl("DeviBot/devibot/mrs_bool/strike", true);
#endif



        }
      }
      samplesPlayed += onSamples;



    }




  }

  return;



  {




    //  for (mrs_natural t=0; t < onSamples; t++)
    //	{
    //	  r = esitar->thumb;
    //	  thumb(samplesPlayed+t) = r;
    //	}




  }


  return;



  // Write Thumb data
  //  thumb.write("boomchickthumb.plot");

  // Write IOI files
  //  lowtimes.write("lowIOI.txt");
  // hitimes.write("hiIOI.txt");

  vector<mrs_natural>::iterator vi;

  //  return;

  MarSystem* playback = mng.create("Series", "playback");
  MarSystem* mix = mng.create("Fanout", "mix");
  mix->addMarSystem(mng.create("SoundFileSource", "orsrc"));
  mix->updctrl("SoundFileSource/orsrc/mrs_bool/mute", true);
  mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
  mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

  playback->addMarSystem(mix);
  playback->addMarSystem(mng.create("Sum", "sum"));
  playback->addMarSystem(mng.create("SoundFileSink", "adest"));
  playback->addMarSystem(mng.create("AudioSink", "dest"));
  playback->addMarSystem(mng.create("MidiOutput", "devibot"));

  cout << "SOUNDFILESINK srate = " << srate << endl;

  playback->updctrl("Fanout/mix/SoundFileSource/orsrc/mrs_string/filename", sfName);

  string sdname;
  string bdname;

  if (srate == 22050.0)
  {
    sdname = "./sd22k.wav";
    bdname = "./bd22k.wav";
  }
  else 				// assume everything is either 22k or 44.1k
  {
    sdname = "./sd.wav";
    bdname = "./bd.wav";
  }


  samplesPlayed = 0;
  onSamples = playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural lowtindex = 0;
  mrs_natural hitindex = 0;


  playback->updctrl("mrs_real/israte", srate);
  playback->updctrl("SoundFileSink/adest/mrs_string/filename", "boomchick.wav");

  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);


  playback->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/hasData")->to<mrs_bool>())
  {
    if (lowtimes[lowtindex] < samplesPlayed)
    {
      lowtindex++;

      if (lowtindex > 1)

        cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
      // Robot Control
#ifdef MARSYAS_MIDIIO
      playback->updctrl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_GE);
      playback->updctrl("MidiOutput/devibot/mrs_natural/byte3", 50);
      playback->updctrl("MidiOutput/devibot/mrs_natural/byte1", 144);
      playback->updctrl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif

      // Bass Drum Play back
      playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
      playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);
    }

    if (hitimes[hitindex] < samplesPlayed)
    {
      hitindex++;

      // Robot Control
#ifdef MARSYAS_MIDIO
      playback->updctrl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_NA);
      playback->updctrl("MidiOutput/devibot/mrs_natural/byte3", 50);
      playback->updctrl("MidiOutput/devibot/mrs_natural/byte1", 144);
      playback->updctrl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif

      // Snare Drum PlayBack
      playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_string/filename", sdname);
      playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_natural/pos", 0);
    }
    playback->tick();
    samplesPlayed += onSamples;
  }

  cout << "FINISHED PROCESSING " << endl;
  cout << "audacity " << sfName << " lowband.wav plowband.wav hiband.wav phiband.wav" << endl;
  delete hidest;
  delete lowdest;
  delete total;
}




// Play a collection l of soundfiles
void tempo(string inFname, string outFname, string method)
{

  MRSDIAG("tempo.cpp - tempo");


  string resName;
  string sfName;


  // For each file in collection estimate tempo
  sfName = inFname;
  resName = outFname;

  if (method == "BOOMCHICK_WAVELET")
  {
    cout << "BOOM-CHICK Wavelet RHYTHM EXTRACTION method " << endl;
    tempo_bcWavelet(sfName, resName);
  }
  else if (method == "BOOMCHICK_FILTER")
  {
    cout << "BOOM-CHICK Filter RHYTHM EXTRACTION method " << endl;
    tempo_bcFilter(sfName, resName);
  }

  else
    cout << "Unsupported tempo induction method " << endl;


}



void
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
  ifstream from1(name.c_str());
  mrs_natural attempts  =0;


  MRSDIAG("Trying current working directory: " + name);
  if (from1.good() == false)
  {
    attempts++;
  }
  else
  {
    from1 >> l;
    l.setName(name.substr(0, name.rfind(".", name.length())));
  }


  if (attempts == 1)
  {
    string warn;
    warn += "Problem reading collection ";
    warn += name;
    warn += " - tried both default mf directory and current working directory";
    MRSWARN(warn);
    exit(1);

  }
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("start", "s", 0.0f);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addRealOption("length", "l", 1000.0f);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addRealOption("repetitions", "r", 1.0);
  cmd_options.addStringOption("method", "m", EMPTYSTRING);
  cmd_options.addRealOption("band", "b", 0.0);

}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  repetitions = cmd_options.getRealOption("repetitions");
  gain = cmd_options.getRealOption("gain");
  pluginName = cmd_options.getStringOption("plu1gin");
  fileName   = cmd_options.getStringOption("filename");
  methodopt = cmd_options.getStringOption("method");
  bandopt = (mrs_natural)cmd_options.getRealOption("band");
}





int
main(int argc, const char **argv)
{

  MRSDIAG("tempo.cpp - main");

  string progName = argv[0];
  if (argc == 1)
  {
    printUsage(progName);
    exit(1);
  }

  initOptions();
  cmd_options.readOptions(argc,argv);
  loadOptions();


  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;


  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);


  string method;

  if (methodopt == EMPTYSTRING)
    method = "NEW";
  else
    method = methodopt;




  tempo(soundfiles[0], soundfiles[1], method);
  exit(0);
}






