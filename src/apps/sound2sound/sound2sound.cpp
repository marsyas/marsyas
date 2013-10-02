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


//
// sound2sound
//
// A program that uses Marsyas to do various types of audio
// processing/digital audio effects that takes as input a single
// audio file and generate a single audio file that is the result of
// the processing.
//
// written by gtzan (c) 2010 - GPL - gtzan@cs.uvic.ca
//

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/FileName.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>


#include <vector>
#include <iomanip>

using namespace std;
using namespace Marsyas;

//
// Global variables for various commandline options
//
int helpopt;
int usageopt;
int verboseopt;
mrs_real gain;
mrs_real targetrms;

mrs_real frequency;
mrs_real qfactor;
mrs_real start;
mrs_real length;

mrs_string mode;
CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("sound2sound.cpp - printUsage");
  cerr << "Usage : " << progName << " in.wav [out.wav]" << endl;
  cerr << endl;
  cerr << "where : " << endl;
  cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
  cerr << "   out.wav is the optional output filename" << endl;
  cerr << progName << "-h provides more information" << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("sound2sound.cpp - printHelp");
  cerr << "sound2sound" << endl;
  cerr << "------------------------------------------------------"<< endl;

  cerr << "written by gtzan (c) 2010 GPL - gtzan@cs.uvic.ca" << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " in.wav [out.wav]" << endl;
  cerr << endl;
  cerr << "where : " << endl;
  cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
  cerr << "   out.wav is the optional name of the audio file to be generated" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage       : display short usage info" << endl;
  cerr << "-h --help        : display this information " << endl;
  cerr << "-v --verbose     : verbose output" << endl;
  cerr << "------------------------------------------------------"<< endl;
  cerr << "-m --mode        : mode which can be one of [bandpass, highpass, lowpass]" << endl;
  cerr << "-s --start       : start in seconds" << endl;
  cerr << "-l --length      : length in seconds" << endl;

  cerr << "-g --gain        : gain" << endl;
  cerr << "-tr --target_rms : scale so that output has global target RMS" << endl;
  cerr << "-f --frequency   : frequency" << endl;
  cout << "-q --qfactor     : qfacotr" << endl;
  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addRealOption("frequency", "f", 500);
  cmd_options.addRealOption("qfactor", "q", 1.0);
  cmd_options.addStringOption("mode" , "m", "bandpass");
  cmd_options.addRealOption("length", "l", -1.0);
  cmd_options.addRealOption("start", "s", 0.0);
  cmd_options.addRealOption("target_rms", "tr", 0.5);
}


void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  gain = cmd_options.getRealOption("gain");
  frequency = cmd_options.getRealOption("frequency");
  qfactor = cmd_options.getRealOption("qfactor");
  mode = cmd_options.getStringOption("mode");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  targetrms = cmd_options.getRealOption("target_rms");
}





void
process(string ifname, string ofname, string mode)
{
  FileName inFile(ifname);
  if (ofname == "default.wav")
    ofname = inFile.nameNoExt() + "Output.wav";

  if (verboseopt)
  {
    cout << "***** sound2sound *****" << endl;
    cout << "Input filename  = " << ifname << endl;
    cout << "Output filename = " << ofname << endl;
    cout << "Mode            = " << mode << endl;
    cout << "Start           = " << start << " seconds" << endl;
    if (length == -1.0)
      cout << "Length          = full duration" << endl;
    else
      cout << "Length          = " << length << " seconds" << endl;
    cout << "Gain            = " << gain << endl;
    cout << "Frequency       = " << frequency << endl;
    cout << "Q-factor        = " << qfactor << endl;

  }

  MarSystemManager mng;
  mrs_natural nChannels;
  mrs_real srate;
  mrs_natural offset;


  if (mode == "normalize")
  {
    MarSystem* rmsnet = mng.create("Series/rmsnet");
    rmsnet->addMarSystem(mng.create("SoundFileSource/src"));
    rmsnet->addMarSystem(mng.create("Rms/rms"));

    rmsnet->updControl("SoundFileSource/src/mrs_string/filename", ifname);
    rmsnet->updControl("mrs_natural/inSamples", 4096);

    nChannels = rmsnet->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
    srate = rmsnet->getctrl("mrs_real/israte")->to<mrs_real>();
    offset = (mrs_natural) (start * srate * nChannels);

    rmsnet->updControl("SoundFileSource/src/mrs_natural/pos", offset);
    rmsnet->updControl("SoundFileSource/src/mrs_natural/loopPos", offset);
    rmsnet->updControl("SoundFileSource/src/mrs_real/duration", length);

    mrs_realvec rms(1);
    mrs_real max_rms = 0.0;

    while (rmsnet->getControl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    {
      rmsnet->tick();
      rms = rmsnet->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
      if (rms(0) >= max_rms)
        max_rms = rms(0);
    }

    mrs_real gain = targetrms / max_rms;

    MarSystem* gainnet = mng.create("Series/gainnet");
    gainnet->addMarSystem(mng.create("SoundFileSource/src"));
    gainnet->addMarSystem(mng.create("Gain/gain"));
    gainnet->addMarSystem(mng.create("SoundFileSink/dest"));

    gainnet->updControl("SoundFileSource/src/mrs_string/filename", ifname);
    gainnet->updControl("SoundFileSink/dest/mrs_string/filename", ofname);
    gainnet->updControl("Gain/gain/mrs_real/gain", gain);

    nChannels = gainnet->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
    srate = gainnet->getctrl("mrs_real/israte")->to<mrs_real>();
    offset = (mrs_natural) (start * srate * nChannels);

    gainnet->updControl("SoundFileSource/src/mrs_natural/pos", offset);
    gainnet->updControl("SoundFileSource/src/mrs_natural/loopPos", offset);
    gainnet->updControl("SoundFileSource/src/mrs_real/duration", length);

    while (gainnet->getControl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    {
      gainnet->tick();
    }
  }
  else if (mode == "resample")
  {
    if (verboseopt)
      cout << "Resampling mode" << endl;

  }

  else
  {
    MarSystem* net = mng.create("Series/net");
    net->addMarSystem(mng.create("SoundFileSource/src"));
    net->addMarSystem(mng.create("Biquad/filter"));
    net->addMarSystem(mng.create("Gain/gain"));
    net->addMarSystem(mng.create("SoundFileSink/dest"));

    net->updControl("SoundFileSource/src/mrs_string/filename", ifname);
    net->updControl("SoundFileSink/dest/mrs_string/filename", ofname);


    nChannels = net->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
    srate = net->getctrl("mrs_real/israte")->to<mrs_real>();
    offset = (mrs_natural) (start * srate * nChannels);

    net->updControl("SoundFileSource/src/mrs_natural/pos", offset);
    net->updControl("SoundFileSource/src/mrs_natural/loopPos", offset);
    net->updControl("SoundFileSource/src/mrs_real/duration", length);


    net->updControl("Biquad/filter/mrs_real/frequency", frequency);
    net->updControl("Biquad/filter/mrs_real/resonance", qfactor);
    net->updControl("Gain/gain/mrs_real/gain", gain);
    net->updControl("Biquad/filter/mrs_string/type", mode);

    while (net->getControl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    {
      net->tick();
    }

  }

}



int
main(int argc, const char **argv)
{
  MRSDIAG("sound2sound.cpp - main");

  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> files = cmd_options.getRemaining();
  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);

  if (files.size() == 1)
  {
    cout << "Using default output filename" << endl;
    files.push_back("default.wav");
  }

  process(files[0], files[1], mode);

  exit(0);


}






