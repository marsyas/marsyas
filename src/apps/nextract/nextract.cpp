/*
** Copyright (C) 2013 Steven Ness <sness@sness.net>
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
   nextract: new feature extraction
*/

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector> 
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "common_source.h"

using namespace std;
using namespace Marsyas;

int helpopt;
int usageopt;
mrs_natural memSize = 40;
mrs_natural winSize = 512;
mrs_natural hopSize = 512;
string outputName = EMPTYSTRING;
mrs_bool timeOutput = false;

mrs_bool mfccFeature = false;
mrs_bool centroidFeature = false;
mrs_bool rolloffFeature = false;
mrs_bool fluxFeature = false;

mrs_bool yinFeature = false;

mrs_string outputFormat = "libsvm";

CommandLineOptions cmd_options;

int printUsage(string progName)
{
  MRSDIAG("nextract.cpp - printUsage");
  cerr << "Usage : " << progName << " collection" << endl;
  cerr << endl;
  return 0;
}

int
printHelp(string progName)
{
  MRSDIAG("nextract.cpp - printHelp");
  cerr << "nextract, MARSYAS, Copyright 2013 Steven Ness" << endl;
  cerr << "---------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " file1 file2 ... fileN" << endl;
  cerr << endl;
  cerr << "where file1 file2 ... fileN are in a Marsyas supported format." << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u       --usage                                : Display short usage info." << endl;
  cerr << "-v       --verbose                              : Verbose output." << endl;
  cerr << "-m       --memory                               : Memory size." << endl;
  cerr << "-ws      --windowsize                           : Analysis window size in samples." << endl;
  cerr << "-hp      --hopsize                              : Analysis hop size in samples." << endl;
  cerr << "-t       --timeOutput                           : Output the current time as the label" << endl;
  cerr << "-mfcc    --melFrequencyCepstralCoefficients     : Output MFCC features." << endl;
  cerr << "-ctd     --centroid                             : Output spectral centroid features." << endl;
  cerr << "-rlf     --rolloff                              : Output spectral rolloff features." << endl;
  cerr << "-flx     --flux                                 : Output spectral flux features." << endl;

  // cerr << "-chroma  --chroma                               : Output chroma features." << endl;
  // cerr << "-sfm     --spectralFlatnessMeasure              : Output spectral flatness measure features." << endl;
  // cerr << "-scf     --spectralCrestFactor                  : Output spectral crest factor features." << endl;
  // cerr << "-zcrs    --zeroCrossings                        : Output zero crossings as a feature." << endl;
  // cerr << "-lsp     --lineSpectralPair                     : Output line spectral pair features." << endl;
  // cerr << "-lpcc    --linearPredictionCepstralCoefficients : Output linear prediction cepstral coefficients features." << endl;
  // cerr << "-yin     --yin                                  : Output yin pitch estimate as a feature." << endl;

  cerr << "-o       --output                               : File to save data to." << endl;
  cerr << "-of       --outputFormat                               : Output file format (libsvm, sonicvisualiser)" << endl;
  
  return 0;
}


void initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addBoolOption("timeOutput", "t", false);

  cmd_options.addNaturalOption("memory", "m", 20);
  cmd_options.addNaturalOption("windowsize", "ws", 512);
  cmd_options.addNaturalOption("hopsize", "hp", 512);

  cmd_options.addBoolOption("melFrequencyCepstralCoefficients","mfcc", false);
  cmd_options.addBoolOption("centroid","ctd", false);
  cmd_options.addBoolOption("rolloff","rlf", false);
  cmd_options.addBoolOption("flux","flx", false);

  // cmd_options.addBoolOption("chroma", "chroma", false);
  // cmd_options.addBoolOption("spectralFlatnessMeasure","sfm", false);
  // cmd_options.addBoolOption("spectralCrestFactor","scf", false);
  // cmd_options.addBoolOption("spectralFeature", "spfe", false);
  // cmd_options.addBoolOption("zeroCrossings", "zcrs", false);
  // cmd_options.addBoolOption("lineSpectralPair", "lsp", false);
  // cmd_options.addBoolOption("linearPredictionCepstralCoefficients", "lpcc", false);
  
  cmd_options.addBoolOption("yin", "yin", false);

  cmd_options.addStringOption("output", "o", EMPTYSTRING);
  cmd_options.addStringOption("outputFormat", "of", "libsvm");
}

void loadOptions()
{
  cout << "loadOptions Window Size (in samples): " << winSize << endl;

  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  timeOutput = cmd_options.getBoolOption("timeOutput");
  memSize = cmd_options.getNaturalOption("memory");
  winSize = cmd_options.getNaturalOption("windowsize");
  hopSize = cmd_options.getNaturalOption("hopsize");
  mfccFeature = cmd_options.getBoolOption("mfcc");
  centroidFeature = cmd_options.getBoolOption("centroid");
  rolloffFeature = cmd_options.getBoolOption("rolloff");
  fluxFeature = cmd_options.getBoolOption("flux");
  yinFeature = cmd_options.getBoolOption("yin");

  outputName = cmd_options.getStringOption("output");
  outputFormat = cmd_options.getStringOption("outputFormat");

  // If no features were explicitly set, out a small standard set of features
  if ((mfccFeature == false) && 
      (centroidFeature == false) &&
      (rolloffFeature == false) &&
      (fluxFeature == false) &&
      (yinFeature == false)) {
        mfccFeature = true;
  }

}

void extract(string inCollectionName)
{
  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("ShiftInput/si"));
  net->addMarSystem(mng.create("Windowing", "ham"));
  net->addMarSystem(mng.create("Spectrum", "spk"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspk"));

  MarSystem* spectralFeatureFanout = mng.create("Fanout", "spectralFeatureFanout");
  if (mfccFeature) {
    spectralFeatureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  }

  if (centroidFeature) {
    spectralFeatureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  }

  if (rolloffFeature) {
    spectralFeatureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  }

  if (fluxFeature) {
    spectralFeatureFanout->addMarSystem(mng.create("Flux", "flux"));
  }
  net->addMarSystem(spectralFeatureFanout);
  

  if(memSize != 0) {
	net->addMarSystem(mng.create("TextureStats", "tStats"));
	net->updControl("TextureStats/tStats/mrs_natural/memSize", memSize);
  }

  net->updControl("SoundFileSource/src/mrs_string/filename", inCollectionName);
  net->updControl("mrs_natural/inSamples", hopSize);
  net->updControl("ShiftInput/si/mrs_natural/winSize", winSize);

  ofstream ofs;
  if (outputName != EMPTYSTRING) {
    ofs.open(outputName.c_str());
  }

  mrs_realvec data;
  int numTicks = 0;
  float currentTime;
  float sampleRate = net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();

  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() ){
    net->tick();
    numTicks++;

    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    currentTime = (numTicks * hopSize) / sampleRate;

    if (outputName != EMPTYSTRING) {

      if (timeOutput) {
        ofs << currentTime << " ";
      } else {
        ofs << net->getctrl("SoundFileSource/src/mrs_real/currentLabel")->to<mrs_real>() << " ";
      }

      for (int i = 1; i < data.getRows(); i++) {

        if (outputFormat == "sonicvisualiser") {
          ofs << data(i, 0) << " ";
        } else {
          ofs << i << ":" << data(i, 0) << " ";
        }
      }
      ofs << endl;
    }


  }

  if (outputName != EMPTYSTRING) {
    ofs.close();
  }


}
  

int
main(int argc, const char **argv)
{
  string progName = argv[0];

  if (argc == 1)
    {
      printUsage(progName);
      return 0;
    }

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  if (helpopt)
    return printHelp(progName);

  if (usageopt)
    return printUsage(progName);

  cout << endl;
  cout << "Window Size (in samples): " << winSize << endl;
  cout << "Hop Size (in samples): " << hopSize << endl;
  cout << "Memory Size (in analysis windows):" << memSize << endl;

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) {
    string sfname = *sfi;
    extract(sfname);
  }

  exit(0);

}
