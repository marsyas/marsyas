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
#include <iomanip>
#include <vector>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

using namespace std;
using namespace Marsyas;

int helpOpt_;
int usageOpt_;
mrs_natural memSize_ = 40;
mrs_natural winSize_ = 512;
mrs_natural hopSize_ = 512;
mrs_bool stereo_ = false;

string outputFilename_ = EMPTYSTRING;
mrs_bool timeOutput_ = false;

mrs_bool mfccFeature_ = false;
mrs_bool centroidFeature_ = false;
mrs_bool rolloffFeature_ = false;
mrs_bool fluxFeature_ = false;
mrs_bool kurtosisFeature_ = false;
mrs_bool skewnessFeature_ = false;
mrs_bool sfmFeature_ = false;
mrs_bool scfFeature_ = false;
mrs_bool chromaFeature_ = false;

mrs_bool rmsFeature_ = false;
mrs_bool yinFeature_ = false;

mrs_bool spectrogramFeature_ = false;
mrs_natural spectrogramFrames_ = 256;
mrs_string spectrogramType_ = "decibels";

mrs_bool saiFeature_ = false;
mrs_string saiFilterbank_ = "pzfc";
mrs_natural saiBoxSizeSpectral_ = 16;
mrs_natural saiBoxSizeTemporal_ = 32;

mrs_natural numMfccs_ = 13;

mrs_natural downsample_ = 1;
string wekaFilename_ = EMPTYSTRING;

mrs_string outputFormat_ = "libsvm";

CommandLineOptions cmdOptions_;

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
  cerr << "-u       --usage        : Display short usage info." << endl;
  cerr << "-v       --verbose      : Verbose output." << endl;
  cerr << "-m       --memory       : Memory size." << endl;
  cerr << "-ws      --windowsize   : Analysis window size in samples." << endl;
  cerr << "-hp      --hopsize      : Analysis hop size in samples." << endl;
  cerr << "         --stereo       : Do analysis on stereo (default convert to mono)." << endl;

  cerr << "         --timeOutput   : Output the current time as the label" << endl;
  cerr << "         --mfcc         : Output MFCC features." << endl;
  cerr << "         --centroid     : Output spectral centroid features." << endl;
  cerr << "         --rolloff      : Output spectral rolloff features." << endl;
  cerr << "         --flux         : Output spectral flux features." << endl;
  cerr << "         --kurtosis     : Output spectral kurtosis features." << endl;
  cerr << "         --skewness     : Output spectral skewness features." << endl;
  cerr << "         --sfm          : Output spectral flatness measure features." << endl;
  cerr << "         --scf          : Output spectral crest factor features." << endl;
  cerr << "         --chroma       : Output chroma features." << endl;

  cerr << "         --numMfccs     : Number of MFCCoefficients to output." << endl;

  // cerr << "-zcrs    --zeroCrossings                        : Output zero crossings as a feature." << endl;
  // cerr << "-lsp     --lineSpectralPair                     : Output line spectral pair features." << endl;
  // cerr << "-lpcc    --linearPredictionCepstralCoefficients : Output linear prediction cepstral coefficients features." << endl;

  cerr << "-rms     --rms                                  : Output rms as a feature." << endl;
  cerr << "-yin     --yin                                  : Output yin pitch estimate as a feature." << endl;

  cerr << "         --spectrogram                          : Output spectrogram as a feature" << endl;
  cerr << "         --spectrogramFrames                    : Frames of power spectrum to use for spectrogram" << endl;
  cerr << "         --spectrogramType                      : Type of spectrogram to output (decibels/magnitude)" << endl;

  cerr << "         --sai                                  : Output Stabilised Auditory Image as a feature." << endl;
  cerr << "         --saiFilterbank                        : Type of filterbank to use for SAI (pzfc, gammatone, carfac)" << endl;
  cerr << "         --saiBoxSizeSpectral                   : Vertical (spectral) size of boxes for SAI boxcutting" << endl;
  cerr << "         --saiBoxSizeTemporal                   : Horizontal (temporal) size of boxes for SAI boxcutting" << endl;

  cerr << "-ds      --downsample                                  : Downsampling ratio" << endl;

  cerr << "-o       --outputFilename                             : File to save data to." << endl;
  cerr << "-of      --outputFormat                                : Output file format (libsvm, sonicvisualiser)" << endl;
  cerr << "-w       --wekafile                : weka .arff filename " << endl;

  return 0;
}


void initOptions()
{
  cmdOptions_.addBoolOption("help", "h", false);
  cmdOptions_.addBoolOption("usage", "u", false);
  cmdOptions_.addBoolOption("verbose", "v", false);
  cmdOptions_.addBoolOption("timeOutput", "", false);

  cmdOptions_.addNaturalOption("memory", "m", 20);
  cmdOptions_.addNaturalOption("windowsize", "ws", 512);
  cmdOptions_.addNaturalOption("hopsize", "hp", 512);

  cmdOptions_.addBoolOption("stereo", "st", false);

  cmdOptions_.addBoolOption("mfcc","", false);
  cmdOptions_.addBoolOption("centroid","", false);
  cmdOptions_.addBoolOption("rolloff","", false);
  cmdOptions_.addBoolOption("flux","", false);
  cmdOptions_.addBoolOption("kurtosis","", false);
  cmdOptions_.addBoolOption("skewness","", false);
  cmdOptions_.addBoolOption("sfm","", false);
  cmdOptions_.addBoolOption("scf","", false);
  cmdOptions_.addBoolOption("chroma", "", false);

  cmdOptions_.addNaturalOption("numMfccs", "", 13);

  // cmdOptions_.addBoolOption("zeroCrossings", "zcrs", false);
  // cmdOptions_.addBoolOption("lineSpectralPair", "lsp", false);
  // cmdOptions_.addBoolOption("linearPredictionCepstralCoefficients", "lpcc", false);

  cmdOptions_.addBoolOption("rms", "", false);
  cmdOptions_.addBoolOption("yin", "", false);

  cmdOptions_.addBoolOption("spectrogram", "", false);
  cmdOptions_.addNaturalOption("spectrogramFrames", "", 256);
  cmdOptions_.addStringOption("spectrogramType", "", EMPTYSTRING);

  cmdOptions_.addBoolOption("sai", "", false);
  cmdOptions_.addStringOption("saiFilterbank", "", "pzfc");
  cmdOptions_.addNaturalOption("saiBoxSizeSpectral", "", 16);
  cmdOptions_.addNaturalOption("saiBoxSizeTemporal", "", 32);

  cmdOptions_.addNaturalOption("downsample", "ds", 1);

  cmdOptions_.addStringOption("outputFilename", "o", EMPTYSTRING);
  cmdOptions_.addStringOption("outputFormat", "of", "libsvm");
  cmdOptions_.addStringOption("wekaFilename", "w", EMPTYSTRING);
}

void loadOptions()
{
  helpOpt_ = cmdOptions_.getBoolOption("help");
  usageOpt_ = cmdOptions_.getBoolOption("usage");
  timeOutput_ = cmdOptions_.getBoolOption("timeOutput");
  memSize_ = cmdOptions_.getNaturalOption("memory");
  winSize_ = cmdOptions_.getNaturalOption("windowsize");
  hopSize_ = cmdOptions_.getNaturalOption("hopsize");
  stereo_ = cmdOptions_.getBoolOption("stereo");
  mfccFeature_ = cmdOptions_.getBoolOption("mfcc");
  centroidFeature_ = cmdOptions_.getBoolOption("centroid");
  rolloffFeature_ = cmdOptions_.getBoolOption("rolloff");
  fluxFeature_ = cmdOptions_.getBoolOption("flux");
  kurtosisFeature_ = cmdOptions_.getBoolOption("kurtosis");
  skewnessFeature_ = cmdOptions_.getBoolOption("skewness");
  sfmFeature_ = cmdOptions_.getBoolOption("sfm");
  scfFeature_ = cmdOptions_.getBoolOption("scf");
  chromaFeature_ = cmdOptions_.getBoolOption("chroma");

  numMfccs_ = cmdOptions_.getNaturalOption("numMfccs");

  rmsFeature_ = cmdOptions_.getBoolOption("rms");
  yinFeature_ = cmdOptions_.getBoolOption("yin");

  spectrogramFeature_ = cmdOptions_.getBoolOption("spectrogram");
  spectrogramFrames_ = cmdOptions_.getNaturalOption("spectrogramFrames");
  spectrogramType_ = cmdOptions_.getStringOption("spectrogramType");

  saiFeature_ = cmdOptions_.getBoolOption("sai");
  saiFilterbank_ = cmdOptions_.getStringOption("saiFilterbank");
  saiBoxSizeSpectral_ = cmdOptions_.getNaturalOption("saiBoxSizeSpectral");
  saiBoxSizeTemporal_ = cmdOptions_.getNaturalOption("saiBoxSizeTemporal");

  downsample_ = cmdOptions_.getNaturalOption("downsample");

  outputFilename_ = cmdOptions_.getStringOption("outputFilename");
  outputFormat_ = cmdOptions_.getStringOption("outputFormat");
  wekaFilename_ = cmdOptions_.getStringOption("wekaFilename");

  // // If no features were explicitly set, out a small standard set of features
  // if ((mfccFeature_ == false) &&
  //     (centroidFeature_ == false) &&
  //     (rolloffFeature_ == false) &&
  //     (fluxFeature_ == false) &&
  //     (kurtoisFeature_ == false) &&
  //     (skewnessFeature_ == false) &&
  //     (sfmFeature_ == false) &&
  //     (scfFeature_ == false) &&
  //     (chromaFeature_ == false) &&
  //     (rmsFeature_ == false) &&
  //     (yinFeature_ == false)) {
  //   mfccFeature_ = true;
  // }

}

void extract(string inCollectionName)
{
  MarSystemManager mng;

  MarSystem* spectrogramSeries = 0;
  MarSystem* saiSeries = 0;

  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  if (stereo_ == false) {
    cout << "doing stereo2mono" << endl;
    net->addMarSystem(mng.create("Stereo2Mono","toMono"));
  }
  net->addMarSystem(mng.create("ShiftInput/si"));
  net->addMarSystem(mng.create("Windowing", "ham"));

  MarSystem* mainFanout = mng.create("Fanout", "mainFanout");

  // Time based features
  if (rmsFeature_ || yinFeature_) {
    MarSystem* timeSeries = mng.create("Series", "timeSeries");
    MarSystem* timeFanout = mng.create("Fanout", "timeFanout");
    if (rmsFeature_) {
      timeFanout->addMarSystem(mng.create("Rms", "rms"));
    }
    if (yinFeature_) {
      timeFanout->addMarSystem(mng.create("Yin", "yin"));
    }
    timeSeries->addMarSystem(timeFanout);
    mainFanout->addMarSystem(timeSeries);
  }

  // Spectrum based features
  if (mfccFeature_ || centroidFeature_ || rolloffFeature_ || fluxFeature_ ||
      kurtosisFeature_ || skewnessFeature_ || sfmFeature_ || scfFeature_ ||
      chromaFeature_) {
    MarSystem* spectralSeries = mng.create("Series", "spectralSeries");
    spectralSeries->addMarSystem(mng.create("Spectrum", "spk"));
    spectralSeries->addMarSystem(mng.create("PowerSpectrum", "pspk"));

    MarSystem* spectralFanout = mng.create("Fanout", "spectralFanout");
    if (mfccFeature_) {
      spectralFanout->addMarSystem(mng.create("MFCC", "mfcc"));
      spectralFanout->updControl("MFCC/mfcc/mrs_natural/coefficients", numMfccs_);
    }

    if (centroidFeature_) {
      spectralFanout->addMarSystem(mng.create("Centroid", "centroid"));
    }

    if (rolloffFeature_) {
      spectralFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
    }

    if (fluxFeature_) {
      spectralFanout->addMarSystem(mng.create("Flux", "flux"));
    }

    if (kurtosisFeature_) {
      spectralFanout->addMarSystem(mng.create("Kurtosis", "kurtosis"));
    }

    if (skewnessFeature_) {
      spectralFanout->addMarSystem(mng.create("Skewness", "skewness"));
    }

    if (sfmFeature_) {
      spectralFanout->addMarSystem(mng.create("SFM", "sfm"));
    }

    if (scfFeature_) {
      spectralFanout->addMarSystem(mng.create("SCF", "scf"));
    }

    if (chromaFeature_) {
      MarSystem* chromaPrSeries = mng.create("Series", "chromaPrSeries");
      chromaPrSeries->addMarSystem(mng.create("Spectrum2Chroma", "chroma"));
      chromaPrSeries->addMarSystem(mng.create("PeakRatio","pr"));
      spectralFanout->addMarSystem(chromaPrSeries);
    }

    spectralSeries->addMarSystem(spectralFanout);
    mainFanout->addMarSystem(spectralSeries);
  }

  // Spectrogram Features
  if (spectrogramFeature_) {
    spectrogramSeries = mng.create("Series", "spectrogramSeries");
    spectrogramSeries->addMarSystem(mng.create("Spectrum", "spk"));
    spectrogramSeries->addMarSystem(mng.create("PowerSpectrum", "pspk"));
    spectrogramSeries->addMarSystem(mng.create("Memory", "spectrogramMemory"));

    mainFanout->addMarSystem(spectrogramSeries);
  }

  // Stabilised Auditory Image Features
  if (saiFeature_) {
    saiSeries = mng.create("Series", "saiSeries");

    if (saiFilterbank_ == "carfac") {
      saiSeries->addMarSystem(mng.create("CARFAC", "carfac"));
    } else if (saiFilterbank_ == "gammatone") {
      saiSeries->addMarSystem(mng.create("AimGammatone", "aimgammatone"));
    } else {
      saiSeries->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
    }
    saiSeries->addMarSystem(mng.create("AimHCL", "aimhcl"));
    saiSeries->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
    saiSeries->addMarSystem(mng.create("AimSAI", "aimsai"));
    saiSeries->addMarSystem(mng.create("AimBoxes", "aimboxes"));
    mainFanout->addMarSystem(saiSeries);
  }

  net->addMarSystem(mainFanout);

  if(memSize_ != 0) {
    net->addMarSystem(mng.create("TextureStats", "tStats"));
    net->updControl("TextureStats/tStats/mrs_natural/memSize", memSize_);
  }

  if (wekaFilename_ != EMPTYSTRING) {
    net->addMarSystem(mng.create("Annotator", "annotator"));
    net->addMarSystem(mng.create("WekaSink", "wsink"));
  }

  net->linkControl("mrs_real/currentLabel",
                   "SoundFileSource/src/mrs_real/currentLabel");
  net->linkControl("mrs_natural/nLabels",
                   "SoundFileSource/src/mrs_natural/nLabels");
  net->linkControl("mrs_string/labelNames",
                   "SoundFileSource/src/mrs_string/labelNames");
  net->linkControl("mrs_string/currentlyPlaying",
                   "SoundFileSource/src/mrs_string/currentlyPlaying");

  if (wekaFilename_ != EMPTYSTRING) {
    net->linkControl("WekaSink/wsink/mrs_string/currentlyPlaying",
                     "mrs_string/currentlyPlaying");
    net->linkControl("WekaSink/wsink/mrs_string/labelNames",
                     "mrs_string/labelNames");
    net->linkControl("WekaSink/wsink/mrs_natural/nLabels",
                     "mrs_natural/nLabels");
    net->linkControl("Annotator/annotator/mrs_real/label",
                     "mrs_real/currentLabel");
  }

  net->updControl("SoundFileSource/src/mrs_string/filename", inCollectionName);
  net->updControl("mrs_natural/inSamples", hopSize_);
  net->updControl("ShiftInput/si/mrs_natural/winSize", winSize_);

  if (spectrogramFeature_) {
    spectrogramSeries->updControl("PowerSpectrum/pspk/mrs_string/spectrumType", spectrogramType_);
    spectrogramSeries->updControl("Memory/spectrogramMemory/mrs_natural/memSize", spectrogramFrames_);
  }

  if (saiFeature_) {
    saiSeries->updControl("AimBoxes/aimboxes/mrs_natural/box_size_spectral", saiBoxSizeSpectral_);
    saiSeries->updControl("AimBoxes/aimboxes/mrs_natural/box_size_temporal", saiBoxSizeTemporal_);
  }

  if (wekaFilename_ != EMPTYSTRING) {
    net->updControl("WekaSink/wsink/mrs_string/filename", wekaFilename_);
    net->updControl("WekaSink/wsink/mrs_natural/downsample", downsample_);
  }

  ofstream ofs;
  if (outputFilename_ != EMPTYSTRING) {
    ofs.open(outputFilename_.c_str());
  }

  mrs_realvec data;
  int numTicks = 0;
  float currentTime;
  float sampleRate = (float) net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();

  mrs_string previouslyPlaying, currentlyPlaying;
  MarControlPtr ctrl_previouslyPlaying = net->getctrl("mrs_string/previouslyPlaying");
  MarControlPtr ctrl_currentlyPlaying = net->getctrl("mrs_string/currentlyPlaying");

  int i = 0;
  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() ) {
    net->tick();
    numTicks++;

    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    if (wekaFilename_ != EMPTYSTRING) {
      net->updControl("WekaSink/wsink/mrs_string/injectComment", "% filename " + currentlyPlaying);
    }

    currentTime = (numTicks * hopSize_) / sampleRate;

    if (outputFilename_ != EMPTYSTRING) {
      if (i % downsample_ == 0) {
        if (timeOutput_) {
          ofs << currentTime << " ";
        } else {
          ofs << net->getctrl("SoundFileSource/src/mrs_real/currentLabel")->to<mrs_real>() << " ";
        }

        // Don't include the column with the label, added by Annotator
        int numCols = data.getCols();
        int numRows = data.getRows() - 1;
        for (int col = 0; col < numCols; col++) {
          for (int row = 0; row < numRows; row++) {

            if (outputFormat_ == "libsvm") {
              ofs << (col * numRows) + row + 1 << ":";
            }
            ofs << data(row, col) << " ";
          }
        }
        ofs << endl;
      }
    }

    i++;
  }

  if (outputFilename_ != EMPTYSTRING) {
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
  cmdOptions_.readOptions(argc, argv);
  loadOptions();

  if (helpOpt_)
    return printHelp(progName);

  if (usageOpt_)
    return printUsage(progName);

  cout << endl;
  cout << "Window Size (in samples): " << winSize_ << endl;
  cout << "Hop Size (in samples): " << hopSize_ << endl;
  cout << "Memory Size (in analysis windows):" << memSize_ << endl;

  vector<string> soundfiles = cmdOptions_.getRemaining();
  vector<string>::iterator sfi;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) {
    string sfname = *sfi;
    extract(sfname);
  }

  exit(0);

}
