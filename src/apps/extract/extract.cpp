/*
** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
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

/**
    extract: feature extraction for a single file
*/

#include <cstdio>
#include <cstdlib>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include "Accumulator.h"
#include "Fanout.h"
#include <marsyas/CommandLineOptions.h>

#include <iostream>
#include <string>

using namespace std;
using namespace Marsyas;

int verboseopt;
int helpopt;
int usageopt;
int normopt;

mrs_natural offset = 0;
mrs_natural duration = 1000 * 44100;
mrs_natural memSize = 40;
mrs_real start = 0.0f;
mrs_real length = 30.0f;
mrs_real gain = 1.0f;

string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string filefeaturename = EMPTYSTRING;
string extractorName = EMPTYSTRING;


CommandLineOptions cmd_options;



void
printUsage(string progName)
{
  MRSDIAG("extract.cpp - printUsage");
  cerr << "Usage : " << progName << " soundfile" << endl;
  cerr << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("extract.cpp - printHelp");
  cerr << "extract, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Simple extraction of feature vectors" << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "soundfile1 " << endl;
  cerr << endl;
  cerr << "where soundfile1 is a sound file in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-v --verbose    : verbose output" << endl;
  cerr << "-n --normalize  : enable normalization" << endl;
  cerr << "-s --start      : playback start offset in second" << endl;
  cerr << "-l --length     : playback length in seconds" << endl;
  cerr << "-p --plugin     : output plugin name" << endl;
  cerr << "-e --extractor  : extractor" << endl;
  cerr << "-m --memory     : memory size" << endl;
  cerr << endl;
  exit(1);
}



void
newExtract(string sfName)
{
  MarSystemManager mng;

  // Find proper soundfile format and create SignalSource
  MarSystem *src = mng.create("SoundFileSource", "src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  // Calculate windowed power spectrum and then
  // calculate specific feature sets

  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");

  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout","spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
  spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
  spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));



  // add the feature to spectral shape
  spectralShape->addMarSystem(spectrumFeatures);


  MarSystem* fnet = mng.create("Series", "fnet");

  fnet->addMarSystem(src);
  fnet->addMarSystem(spectralShape);
  fnet->addMarSystem(mng.create("PlotSink", "psink"));
  fnet->updctrl("PlotSink/psink/mrs_bool/sequence", false);
  fnet->updctrl("PlotSink/psink/mrs_string/separator", ",");

  for (int i=0; i < 100; i++)
    fnet->tick();

}









void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addBoolOption("normalize", "n", false);
  cmd_options.addRealOption("start", "s", 0);
  cmd_options.addRealOption("length", "l", 1000.0f);
  cmd_options.addStringOption("extractor", "e", EMPTYSTRING);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addNaturalOption("memory", "m", 40);
}


void
loadOptions()
{
  verboseopt = cmd_options.getBoolOption("verbose");
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  normopt = cmd_options.getBoolOption("normalize");
  pluginName = cmd_options.getStringOption("plugin");
  extractorName = cmd_options.getStringOption("extractor");
}


int
main(int argc, const char **argv)
{
  MRSDIAG("sfplay.cpp - main");

  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);


  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();


  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);

  int i;
  i =0;

  Collection l;

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    string sfname = *sfi;
    newExtract(sfname);
  }

  exit(0);
}






