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
textract: batch feature extraction
*/

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include "Accumulator.h"
#include <marsyas/TimeLine.h>
#include <marsyas/CommandLineOptions.h>
#include <string>

using namespace std;
using namespace Marsyas;

int helpopt;
int usageopt;
int normopt;

mrs_natural offset = 0;
mrs_natural duration = 1000 * 44100;
mrs_natural memSize = 40;
mrs_real start = 0.0;
mrs_real length = 30.0;
mrs_real gain = 1.0;

string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string filefeaturename = EMPTYSTRING;
string extractorName = EMPTYSTRING;
string tlineName = EMPTYSTRING;

CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("textract.cpp - printUsage");
  cerr << "Usage : " << progName << " [-e extractor] [-h help] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-m memory]  [-u usage] collection1 collection2 ... collectionN" << endl;
  cerr << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("textract.cpp - printHelp");
  cerr << "textract, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "[-c collection] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-v --verbose    : verbose " << endl;
  cerr << "-e --extractor  : extractor " << endl;
  cerr << "-o --offset     : playback start offset in samples " << endl;
  cerr << "-n --normalize  : enable normalization" << endl;
  cerr << "-w --weka       : weka .arff filename " << endl;

  /* TODO: Add short description

  cerr << " -f --filefeature : " << endl;

  */

  cerr << "-p --plugin     : output plugin name " << endl;
  cerr << "-d --duration   : playback duration in samples     " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << "-m --memory  r  : memory size " << endl;
  cerr << endl;
  cerr << "Available extractors: " << endl;
  cerr << "STFT: Centroid, Rolloff Flux, ZeroCrossings " << endl;
  cerr << "MFCC: Mel-frequency Cepstral Coefficients " << endl;
  cerr << "SCF : Spectral Crest Factor (MPEG-7) " << endl;
  cerr << "SFM : Spectral Flatness Measure (MPEG-7) " << endl;
  cerr << endl;
  cerr << "All extractors calculate means and variances over a memory size window" << endl;
  cerr << "SV can be appended in front of any extractor to extract a single vector (mean, variances) over a 30-second clip (for example SVSTF) " << endl;

  exit(1);
}

void textract_trainAccumulator(string sfName, mrs_natural offset, mrs_natural duration, mrs_real start, mrs_real length, mrs_real gain, mrs_natural label, string pluginName, string wekafname, mrs_natural memSize, string extractorStr, TimeLine& tline)
{
  MarSystemManager mng;

  MRSDIAG("sfplay.cpp - sfplay");

  // default
  if (extractorStr == EMPTYSTRING)
    extractorStr = "STFT";

  // Find proper soundfile format and create SignalSource
  MarSystem *src = mng.create("SoundFileSource", "src");

  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);

  if (tlineName == EMPTYSTRING)
  {
    mrs_natural hops = src->getctrl("mrs_natural/size")->to<mrs_natural>() * src->getctrl("mrs_natural/nChannels")->to<mrs_natural>() / 2048 + 1;
    tline.regular(100, hops);
  }

  MarSystem *dest_;
  dest_ = mng.create("AudioSink", "dest");

  MarSystem *series = mng.create("Series", "playbacknet");
  series->addMarSystem(src);
  series->addMarSystem(dest_);

  series->updctrl("AudioSink/dest/mrs_natural/nChannels",
                  series->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>());

  // Calculate duration, offset parameters if necessary
  if (start > 0.0f)
    offset = (mrs_natural) (start
                            * src->getctrl("mrs_real/israte")->to<mrs_real>()
                            * src->getctrl("mrs_natural/nChannels")->to<mrs_natural>());
  if (length != 30.0f)
    duration = (mrs_natural) (length
                              * src->getctrl("mrs_real/israte")->to<mrs_real>()
                              * src->getctrl("mrs_natural/nChannels")->to<mrs_natural>());

  // accumulate feature vectors over 30 seconds
  MarSystem* acc = mng.create("Accumulator", "acc");

  acc->updctrl("mrs_natural/nTimes", 100);

  // Calculate windowed power spectrum and then
  // calculate specific feature sets
  MarSystem* spectralShape = mng.create("Series", "spectralShape");
  spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
  spectralShape->addMarSystem(mng.create("Spectrum","spk"));
  spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  spectralShape->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");

  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");

  if (extractorStr == "STFT")
  {
    spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
    spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
    spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  }
  else if (extractorStr == "STFTMFCC")
  {
    spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
    spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
    spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
    spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
  }
  else if (extractorStr == "MFCC")
    spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
  else if (extractorStr == "SCF")
    spectrumFeatures->addMarSystem(mng.create("SCF", "scf"));
  else if (extractorStr == "SFM")
    spectrumFeatures->addMarSystem(mng.create("SFM", "sfm"));

  mng.registerPrototype("SpectrumFeatures", spectrumFeatures->clone());
  spectralShape->addMarSystem(mng.create("SpectrumFeatures", "spectrumFeatures"));
  mng.registerPrototype("SpectralShape", spectralShape->clone());

  //  add time-domain zerocrossings
  MarSystem* features = mng.create("Fanout", "features");
  features->addMarSystem(mng.create("SpectralShape", "SpectralShape"));
  if (extractorStr == "STFT")
    features->addMarSystem(mng.create("ZeroCrossings", "zcrs"));
  mng.registerPrototype("Features", features->clone());

  // Means and standard deviation (statistics) for texture analysis
  MarSystem* statistics = mng.create("Fanout","statistics");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));
  mng.registerPrototype("Statistics", statistics->clone());

  // weka output
  MarSystem *wsink = mng.create("WekaSink","wsink");

  // Build the overall feature calculation network
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");

  featureNetwork->addMarSystem(src->clone());
  featureNetwork->addMarSystem(mng.create("Features", "features"));
  featureNetwork->addMarSystem(mng.create("Memory", "memory"));
  featureNetwork->updctrl("Memory/memory/mrs_natural/memSize", memSize);
  featureNetwork->addMarSystem(mng.create("Statistics", "statistics"));

  // add network to accumulator
  acc->addMarSystem(featureNetwork->clone());

  // Final network compute 30-second statistics
  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc->clone());
  total->addMarSystem(mng.create("Statistics", "statistics2"));
  // total->addMarSystem(mng.create("Mean", "mn2"));
  total->addMarSystem(wsink->clone());

  // update controls
  total->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  total->updctrl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/mrs_natural/pos", offset);

  mrs_natural wc = 0;
  mrs_natural samplesPlayed =0;

  // main loop for extracting the features
  string className = "";

  total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", sfName);
  wc = 0;
  samplesPlayed = 0;

  total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)3);
  if (wekafname == EMPTYSTRING)
    total->updctrl("WekaSink/wsink/mrs_string/filename", "weka2.arff");
  else
    total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);

  // total->tick();

  for (int r = 0; r < tline.numRegions(); r++)
  {
    cout << "start = " << tline.regionStart(r) << endl;
    cout << "end = " << tline.regionEnd(r) << endl;

    total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos", (mrs_natural)tline.regionStart(r) * tline.lineSize());
    total->updctrl("mrs_natural/inSamples", tline.lineSize());
    if (tlineName == EMPTYSTRING)
    {
      if ((tline.regionClass(r) > 0) && (tline.regionClass(r) != 4))//[?]
      {
        total->updctrl("WekaSink/wsink/mrs_natural/label", tline.regionClass(r)-1);
      }
    }
    else
      total->tick();
  }

  if (pluginName == EMPTYSTRING) // output to stdout
    cout << (*total) << endl;
  else
  {
    ofstream oss(pluginName.c_str());
    oss << (*total) << endl;
  }
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
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addStringOption("wekafile", "w", EMPTYSTRING);
  cmd_options.addStringOption("filefeature", "f", EMPTYSTRING);
  cmd_options.addStringOption("extractor", "e", EMPTYSTRING);
  cmd_options.addNaturalOption("memory", "m", 40);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  normopt = cmd_options.getBoolOption("normalize");

  pluginName = cmd_options.getStringOption("plugin");
  filefeaturename   = cmd_options.getStringOption("filefeature");
  wekafname = cmd_options.getStringOption("wekafile");
  extractorName = cmd_options.getStringOption("extractor");
}

int
main(int argc, const char **argv)
{
  MRSDIAG("sfplay.cpp - main");

  string progName = argv[0];
  progName = progName.erase(0,3);

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  if (argc == 1)
    printUsage(progName);

  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);

  int i = 0;
  Collection l;
  string sfName;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  {
    sfName = *sfi;
    i++;
  }

  string extractorStr = extractorName;
  TimeLine tline;

  if (tlineName == EMPTYSTRING)
  {
    tline.load(tlineName);
    cout << tline;
  }

  if (extractorStr.substr(0,2) == "SV")
  {
    textract_trainAccumulator(sfName, offset, duration, start, length, gain,i, pluginName, wekafname, memSize, extractorName.substr(2, extractorName.length()), tline);
  }

  exit(0);
}








